using Streamer.bot.Plugin.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Stinger : IAdventureEvent
{
    Setting<int> WeightSetting;
    Setting<double> Multiplier;
    Setting<double> AbsoluteDeathChance;
    Setting<double> AbsoluteWinChance;
    Setting<double> MinWinChance;
    Setting<double> MaxWinChance;

    public int Weight { get { return WeightSetting; } }

    public void Init(IInlineInvokeProxy CPH, SettingsManager.Scope scope)
    {
        WeightSetting = scope.At("Weight", 1);
        Multiplier = scope.At("Multiplier", 1.2);
        AbsoluteDeathChance = scope.At("AbsoluteDeathChance", 0.3);
        AbsoluteWinChance = scope.At("AbsoluteWinChance", 0.3);
        MinWinChance = scope.At("MinWinChance", 0.25);
        MaxWinChance = scope.At("MaxWinChance", 0.75);
    }

    public bool CanRun(Adventure.Details details)
    {
        return true;
    }

    private void Win(Adventure.Details details)
    {
        details.Winnings += (int)Math.Round(Multiplier * details.AverageInvestment);
    }

    private void Lose(Adventure.Details details, Adventure.Participant victim)
    {
        details.Winnings += (int)Math.Round(Multiplier * details.AverageInvestment);
        victim.Health = 0;
    }

    public void Run(IInlineInvokeProxy CPH, Adventure.Details details)
    {
        Adventure.Participant victim = details.Participants[details.Progress];
        CPH.SendMessage($"An alpha stinger appears and lunges at {victim}!!");
        CPH.Wait(5000);

        double chance = CPH.NextDouble();

        double lossThreshold = AbsoluteDeathChance;
        double winThreshold = 1.0 - AbsoluteWinChance;

        if (chance < lossThreshold)
        {
            CPH.SendMessage($"The stinger was too quick epiphaDEAD {victim} can't carry on...");
            Lose(details, victim);
        }
        else if (chance >= winThreshold)
        {
            CPH.SendMessage($"{victim} was too quick and got away easily! epiphaGOOD");
            Win(details);
        }
        else
        {
            // winChance: [-1, 1]
            double InvestmentDiff = details.MaxInvestment - details.AverageInvestment;
            double winChance = 0.5;
            if (InvestmentDiff != 0)
            {
                winChance = (victim.Investment - details.AverageInvestment) / (details.MaxInvestment - details.AverageInvestment);
            }
            // [0, 1]
            winChance = (winChance + 1) / 2;
            // [0, Max - Min]
            winChance *= (MaxWinChance - MinWinChance);
            // [Min, Max]
            winChance += MinWinChance;
            double personalThreshold = 1.0 - winChance;
            if (chance < personalThreshold)
            {
                CPH.SendMessage($"{victim} ate all their Paleberries, but it wasn't enough... epiphaDEAD");
                Lose(details, victim);
            }
            else
            {
                CPH.SendMessage($"{victim} ate some Paleberries and the party barely got away epiphaGOOD");
                Win(details);
            }
        }
    }
}
