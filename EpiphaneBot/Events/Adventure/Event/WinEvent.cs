using Streamer.bot.Plugin.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class WinEvent : IAdventureEvent
{
    int Occurrences = 0;
    Setting<int> WeightSetting;
    Setting<double> Multiplier;

    public int Weight { get { return WeightSetting; } }

    public void Init(IInlineInvokeProxy CPH, SettingsManager.Scope scope)
    {
        WeightSetting = scope.At("Weight", 1);
        Multiplier = scope.At("Multiplier", 1.5);
    }

    public bool CanRun(Adventure.Details details)
    {
        return Occurrences < Math.Ceiling((double)details.Participants.Count / 5);
    }

    public void Run(IInlineInvokeProxy CPH, Adventure.Details details)
    {
        Adventure.Participant victim = details.Participants[details.Progress];
        CPH.SendMessage($"{victim} stumbles upon a small treasure trove!! epiphaGOOD");
        details.Winnings += (int)Math.Round(Multiplier * details.AverageInvestment);
    }
}
