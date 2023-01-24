using Streamer.bot.Plugin.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class CrabEvent : IAdventureEvent
{
    Setting<int> RaritySetting;
    Setting<double> Multiplier;
    Setting<double> MinWinChance;
    Setting<double> MaxWinChance;
    Setting<int> AverageWinPlayers;
    Setting<double> OneDeathChance;

    public int Rarity { get { return RaritySetting; } }

    public void Init(IInlineInvokeProxy CPH, SettingsManager.Scope scope)
    {
        RaritySetting = scope.At("Rarity", 1);
        Multiplier = scope.At("Multiplier", 0.3);
        MinWinChance = scope.At("MinWinChance", 0.35);
        MaxWinChance = scope.At("MaxWinChance", 0.7);
        AverageWinPlayers = scope.At("AverageWinPlayers", 3);
        OneDeathChance = scope.At("OneDeathChance", 0.75);
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
        //details.Winnings += (int)Math.Round(Multiplier * details.AverageInvestment);
        victim.Health = 0;
    }

    public void Run(IInlineInvokeProxy CPH, Adventure.Details details)
    {
        Adventure.Participant victim = details.Participants[details.Progress];
        CPH.SendMessage($"Some U6 crabs begin swarming the group!!");
        CPH.Wait(5000);

        double chance = CPH.NextDouble();

        // Survival chance goes up with more players; 0.5 at 3 players (default)
        double playerCountScaled = Math.Min((double)(details.Participants.Count - 1) / ((AverageWinPlayers - 1) * 2), 1.0);

        // Scale from [0.0, 1.0] to the min and max win chances
        double survivalChance = (playerCountScaled * (MaxWinChance - MinWinChance)) + MinWinChance;
        //CPH.LogDebug($"Survival chance: {survivalChance} | RNG: {chance}");
        double survivalThreshold = 1 - survivalChance;

        if (chance >= survivalThreshold)
        {
            if (details.Participants.Count == 1)
            {
                CPH.SendMessage($"Somehow, {victim} fends them off all alone! epiphaGOOD");
            }
            else
            {
                CPH.SendMessage("Everyone teams up and takes down the crabs together! epiphaGOOD");
            }
            Win(details);
        }
        else
        {
            if (details.Progress < 1)
            {
                CPH.SendMessage($"It's too much! {victim} can't handle them all alone epiphaDEAD");
            }
            else
            {
                // 3/4 chance that it's just the one
                chance = chance / survivalThreshold;
                if (chance < OneDeathChance)
                {
                    CPH.SendMessage($"Every crab immediately locks onto {victim}! epiphaDEAD {victim} can't carry on...");
                }
                else
                {
                    int previous = CPH.Between(0, details.Progress - 1);
                    Adventure.Participant victim2 = details.Participants[previous];
                    if (victim2.IsAlive)
                    {
                        CPH.SendMessage($"They just keep spawning more!! epiphaDEAD {victim} and {victim2} can't carry on...");
                        victim2.Health = 0;
                    }
                    else
                    {
                        CPH.SendMessage($"Every crab immediately locks onto {victim}! epiphaDEAD {victim} can't carry on...");
                    }
                }
            }
            Lose(details, victim);
        }
    }
}
