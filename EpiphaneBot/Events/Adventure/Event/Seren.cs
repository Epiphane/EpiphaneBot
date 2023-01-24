using Streamer.bot.Plugin.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class SerenEvent : IAdventureEvent
{
    Setting<int> RaritySetting;
    Setting<float> Multiplier;

    int Occurrences = 0;

    public int Rarity { get { return RaritySetting; } }

    public void Init(IInlineInvokeProxy CPH, SettingsManager.Scope scope)
    {
        RaritySetting = scope.At("Rarity", 25);
        Multiplier = scope.At("Multiplier", 1.25f);
    }

    public bool CanRun(Adventure.Details details)
    {
        bool isNotInGroup = details.Participants.All(p => p.User.Name != "serenprickitous");
        return Occurrences < 1 && isNotInGroup && details.Participants.Count > 1;
    }

    public void Run(IInlineInvokeProxy CPH, Adventure.Details details)
    {
        Occurrences++;

        CPH.SendMessage($"A pioneer shows up! It's... serenprickitous?!");
        CPH.Wait(5000);

        Adventure.Participant victim = details.Participants[0];
        foreach (var player in details.Participants)
        {
            if (player.Investment > victim.Investment && player.Health > 0)
            {
                victim = player;
            }
        }

        CPH.SendMessage($"He kidnaps {victim} and disappears into the foliage!! epiphaDEAD");
        CPH.Wait(5000);

        CPH.SendMessage($"All of {victim}'s caterium is left behind for everyone else...");
        victim.Health = 0;
        details.Winnings += (int)Math.Round(victim.Investment * Multiplier);
    }
}
