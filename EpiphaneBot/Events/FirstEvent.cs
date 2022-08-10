using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;

public class FirstEvent : RPGEvent
{
    private const int firstsAvailable = 3;
    private List<User> claimants = new List<User>();

    public FirstEvent(PetaPoco.IDatabase DB, IInlineInvokeProxy CPH) : base(DB, CPH)
    {
        
    }

    public override void Start()
    {
    }

    private int CalculateReward(int place)
    {
        List<int> rewards = new List<int> { 15, 10, 5 };
        return rewards[place];
    }

    public override bool Handle(User user, string message)
    {
        if (!message.StartsWith("!first"))
        {
            return false;
        }

        if (ClaimFirst(user, out int place))
        {
            int reward = CalculateReward(place);
            List<string> messages = new List<string> { "first", "second", "third" };
            List<string> endings = new List<string> { "2 prizes remaining", "1 prize remaining", "" };
            string placeStr = messages[place];

            user.Gold += reward;
            DB.Save(user);

            CPH.SendMessage($"You did it, {user.Name}! You were {placeStr} to claim the prize today and found {reward} gold! {endings[place]}");
        }
        else if (HasClaimed(user))
        {
            CPH.SendMessage($"You've already used that today, {user.Name}. Nice try ;)");
        }
        else
        {

            CPH.SendMessage($"Sorry {user.Name}, you were beaten by {Claimants} today. Better luck next time!");
        }

        return true;
    }

    public bool ClaimFirst(User user, out int place)
    {
        place = claimants.FindIndex(u => u.Id == user.Id);
        if (place >= 0)
        {
            return false;
        }

        if (claimants.Count < firstsAvailable)
        {
            place = claimants.Count;
            claimants.Add(user);
            return true;
        }

        return false;
    }

    public bool HasClaimed(User user)
    {
        return claimants.Find(u => u.Id == user.Id) != null;
    }

    public string Claimants
    {
        get
        {
            string claimStr = "";
            for (int i = 0; i < claimants.Count; i++)
            {
                claimStr += claimants[i].Name;
                if (i < claimants.Count - 2)
                {
                    claimStr += ", ";
                }
                else if (i == claimants.Count - 2)
                {
                    claimStr += ", and ";
                }
            }
            return claimStr;
        }
    }
}
