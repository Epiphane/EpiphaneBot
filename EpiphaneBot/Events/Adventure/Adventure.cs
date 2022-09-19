using System;
using System.Collections.Generic;
using System.Linq;
using Streamer.bot.Plugin.Interface;
using System.ComponentModel.DataAnnotations;

public class Adventure : RPGEvent
{
    public enum State
    {
        NotStarted,
        Preparing,
        Running,
        Cooldown,
        Done,
    };

    public State state { get; private set; } = State.NotStarted;

    public enum Region
    {
        [Display(Name = "Northern Forest")]
        NorthernForest,

        [Display(Name = "Spire Coast")]
        SpireCoast,

        [Display(Name = "Titan Forest")]
        TitanForest,

        [Display(Name = "Crater Lakes")]
        CraterLakes,

        [Display(Name = "Red Jungle")]
        RedJungle,

        [Display(Name = "Bamboo Fields")]
        BambooFields,

        [Display(Name = "Jungle Spires")]
        JungleSpires,

        [Display(Name = "Swamp")]
        Swamp,

        [Display(Name = "Abyss Cliffs")]
        AbyssCliffs,

        [Display(Name = "Blue Crater")]
        BlueCrater,
    }

    public class Details
    {
        public Region Region;
        public List<Participant> Participants;
        public int Progress = 0;
        public int Winnings = 0;
        public int MaxInvestment = 0;
        public int AverageInvestment = 1;
    }

    public class Participant
    {
        public User User;
        public int Investment;
        public float Claim = 0;
        public int Berries = 0;
        public int Health = 10;
        public int Winnings = 0;

        public override string ToString()
        {
            return User.Name;
        }
    }

    private DateTime CooldownTime;

    private readonly List<Participant> participants = new List<Participant>();

    private readonly Setting<string> CreationMessage;
    private readonly Setting<string> CooldownMessage;
    private readonly Setting<string> CooldownOverMessage;
    private readonly Setting<string> RaidStartMessage;
    private readonly Setting<string> UserJoinMessage;
    private readonly Setting<string> NewInvestmentMessage;
    private readonly Setting<string> AlreadyJoinedMessage;
    private readonly Setting<string> AlreadyRunningMessage;
    private readonly Setting<int> StartDelaySec;
    private readonly Setting<int> EventDelaySec;
    private readonly Setting<int> CooldownSec;

    private readonly List<IAdventureEvent> AvailableEvents = new List<IAdventureEvent>();

    private readonly SettingsManager.Scope EventSettings;

    private Adventure(IInlineInvokeProxy CPH, PetaPoco.IDatabase DB, SettingsManager.Scope settings) : base(DB, CPH)
    {
        EventSettings = settings.GetScope("Event");
        CreationMessage = settings.At("CreationMessage", "{user} is looking to start a raid! To join, type !raid <amount>");
        CooldownMessage = settings.At("CooldownMessage", "It's too soon to start another raid! (cooldown: {cooldown})");
        CooldownOverMessage = settings.At("CooldownOverMessage", "The tavern is back open! To start a raid, type !raid <amount>");
        RaidStartMessage = settings.At("RaidStartMessage", "It's time! Everyone assembles at the HUB and embarks into the {location}...");
        UserJoinMessage = settings.At("UserJoinMessage", "{user} has joined the raid with {amount} {currency}!");
        NewInvestmentMessage = settings.At("NewInvestmentMessage", "{user} changes their investment to {amount} {currency}!");
        AlreadyJoinedMessage = settings.At("AlreadyJoinedMessage", "{user}, you've already joined the raid!");
        AlreadyRunningMessage = settings.At("AlreadyRunningMessage", "The raid is already underway! Best to wait for the next one.");
        StartDelaySec = settings.At("StartDelaySec", 1 * 60);
        EventDelaySec = settings.At("EventDelaySec", 10);
        CooldownSec = settings.At("CooldownSec", 5 * 60);

        RegisterEvents();
    }

    public static void InitEvents(IInlineInvokeProxy CPH, PetaPoco.IDatabase DB, SettingsManager.Scope settings)
    {
        // Create an Adventure, which instantiates all settings and registers events
        new Adventure(CPH, DB, settings);
    }

    private void RegisterEvents()
    {
        AvailableEvents.Clear();

        foreach (Type type in System.Reflection.Assembly.GetExecutingAssembly().GetTypes()
                .Where(type => type.GetInterfaces().Contains(typeof(IAdventureEvent))))
        {
            IAdventureEvent EventType = (IAdventureEvent)Activator.CreateInstance(type);
            EventType.Init(CPH, EventSettings.GetScope(type.Name));
            AvailableEvents.Add(EventType);
        }
    }

    public static Adventure Create(IInlineInvokeProxy CPH, PetaPoco.IDatabase DB, SettingsManager.Scope settings, User user, string[] input)
    {
        Adventure result = new Adventure(CPH, DB, settings);
        if (!result.TryJoin(user, input.Length > 0 ? input[0] : "", out _))
        {
            return null;
        }

        return result;
    }

    public bool ParseInvestment(string message, out int investment)
    {
        investment = 0;
        if (message == "")
        {
            return true;
        }

        if (!int.TryParse(message, out investment))
        {
            CPH.SendMessage($"Invalid investment: ${message}. Type !raid <number> to start a raid");
            return false;
        }

        return true;
    }

    public bool TryJoin(User user, string investment, out int amount)
    {
        amount = 0;
        if (!ParseInvestment(investment, out amount))
        {
            CPH.LogWarn($"Did not add {user} to raid, could not parse investment");
            return false;
        }

        if (user.Caterium < amount)
        {
            CPH.SendMessage($"You don't have enough Caterium! (Current: {user.Caterium})");
            return false;
        }
        else if (amount < 1)
        {
            CPH.SendMessage($"You must invest at least 1 Caterium in this adventure to join!");
            return false;
        }

        Participant existing = participants.Find(signup => signup.User.Id == user.Id);
        if (existing != null)
        {
            if (existing.Investment == amount)
            {
                CPH.SendMessage(AlreadyJoinedMessage.Get()
                    .Replace("{user}", user.Name)
                    );
            }
            else
            {
                existing.Investment = amount;
                CPH.SendMessage(NewInvestmentMessage.Get()
                    .Replace("{user}", user.Name)
                    .Replace("{amount}", $"{amount}")
                    .Replace("{currency}", "Caterium")
                    );
            }
        }
        else
        {
            participants.Add(new Participant()
            {
                User = user,
                Investment = amount
            });
            return true;
        }

        return false;
    }

    public override bool IsDone { get { return state == State.Done; } }

    public void Prepare()
    {
        RegisterEvents();
        state = State.Preparing;

        User creator = participants[0].User;
        CPH.SendMessage(CreationMessage.Get().Replace("{user}", creator.Name));
        CPH.Wait(StartDelaySec * 1000);
    }

    public Details GenerateDetails()
    {
        // Start the adventure
        state = State.Running;

        Helpers.Shuffle(CPH, participants);
        Region[] regions = (Region[])Enum.GetValues(typeof(Region));
        Region region = regions[CPH.Between(0, regions.Length - 1)];
        Details details = new Details
        {
            Region = region,
            Participants = participants,
        };

        return details;
    }

    public void Begin(Details details)
    {
        // Start the adventure
        state = State.Running;

        int totalInvestment = 0;
        participants.ForEach(participant =>
        {
            totalInvestment += participant.Investment;
            participant.User.Caterium -= participant.Investment;
            participant.Berries = participant.Investment / 2;
            details.MaxInvestment = Math.Max(details.MaxInvestment, participant.Investment);
        });

        details.AverageInvestment = totalInvestment / participants.Count;

        CPH.SendMessage($"With your {totalInvestment} caterium, the group purchases some Paleberries for safety...");
        CPH.SendMessage(RaidStartMessage.Get().Replace("{location}", $"{Enum.GetName(typeof(Region), details.Region)}"));
        CPH.Wait(EventDelaySec * 1000);
    }

    public override void Run()
    {
        RegisterEvents();
        Prepare();

        Details details = GenerateDetails();
        Begin(details);

        // Calculate Result
        for (details.Progress = 0; details.Progress < participants.Count; details.Progress++)
        {
            var available = AvailableEvents.Where(e => e.CanRun(details));
            int totalWeight = available.Aggregate(0, (current, e) => current + e.Weight);

            CPH.LogDebug($"Num available: {available.Count()}");
            int chosen = CPH.Between(0, totalWeight);
            CPH.LogDebug($"Chosen weight: {chosen}");
            int index = -1;
            do
            {
                index++;
                IAdventureEvent current = available.ElementAt(index);
                chosen -= current.Weight;
            }
            while (chosen > 0);
            CPH.LogDebug($"Choosing event {index}");
            available.ElementAt(index).Run(CPH, details);

            if (details.Progress < participants.Count - 1)
            {
                CPH.Wait(EventDelaySec * 1000);
            }
        }

        int totalInvestment = 0;
        participants.ForEach(participant =>
        {
            if (participant.Health > 0)
            {
                totalInvestment += participant.Investment;
            }
        });

        List<string> winnings = new List<string>();
        participants.ForEach(participant =>
        {
            if (participant.Health > 0)
            {
                double claim = (double)participant.Investment / totalInvestment;
                participant.Winnings = (int)Math.Ceiling(claim * details.Winnings);
                participant.User.Caterium += participant.Winnings;
                winnings.Add($"{participant} ({participant.Winnings})");
            }

            DB.Save(participant.User);
        });

        var survivors = details.Participants.Where(user => user.Health > 0);
        int numSurvivors = survivors.Count();
        if (numSurvivors == 0)
        {
            CPH.SendMessage($"The adventure is complete, but nobody survived :( All the caterium is lost");
        }
        else if (numSurvivors == 1)
        {
            CPH.SendMessage($"The adventure is complete! Only {survivors.ElementAt(0)} survived.");
            CPH.SendMessage($"{survivors.ElementAt(0)} gets all the {details.Winnings} caterium found...");
        }
        else
        {
            CPH.SendMessage($"The adventure is complete! Survivors: {String.Join(", ", winnings)}");
        }

        CooldownTime = DateTime.Now.AddSeconds(CooldownSec);
        state = State.Cooldown;
        CPH.Wait(CooldownSec * 1000);
        CPH.SendMessage(CooldownOverMessage);
        state = State.Done;
    }

    public override bool Handle(User user, string[] input)
    {
        if (state == State.Preparing)
        {
            if (TryJoin(user, input[0], out int amount))
            {
                CPH.SendMessage(UserJoinMessage.Get()
                    .Replace("{user}", user.Name)
                    .Replace("{amount}", $"{amount}")
                    .Replace("{currency}", "Caterium")
                    );
                return true;
            }
            return false;
        }
        else if (state == State.Running)
        {
            CPH.SendMessage(AlreadyRunningMessage.Get()
                .Replace("{user}", user.Name)
                );
        }
        else if (state == State.Cooldown)
        {
            CPH.SendMessage(CooldownMessage.Get()
                .Replace("{user}", user.Name)
                .Replace("{cooldown}", CooldownTime.Subtract(DateTime.Now).Seconds.ToString())
                );
        }
        else
        {
            CPH.LogWarn($"Unexpected Raid state: {state}");
            CPH.SendMessage("Uh oh, something went wrong D:");
        }

        return false;
    }
}
