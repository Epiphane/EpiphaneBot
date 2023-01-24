using Streamer.bot.Plugin.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class WinEvent : IAdventureEvent
{
    int Occurrences = 0;
    Setting<int> RaritySetting;
    Setting<double> Multiplier;

    public int Rarity { get { return RaritySetting; } }

    public void Init(IInlineInvokeProxy CPH, SettingsManager.Scope scope)
    {
        RaritySetting = scope.At("Rarity", 3);
        Multiplier = scope.At("Multiplier", 1.0);
    }

    public bool CanRun(Adventure.Details details)
    {
        return Occurrences < Math.Ceiling((double)details.Participants.Count / 5);
    }

    public void Run(IInlineInvokeProxy CPH, Adventure.Details details)
    {
        Adventure.Participant victim = details.Participants[details.Progress];
        CPH.SendMessage($"{victim} stumbles upon a hard drive!! epiphaGOOD");
        details.Winnings += (int)Math.Round(Multiplier * details.AverageInvestment);
    }
}
