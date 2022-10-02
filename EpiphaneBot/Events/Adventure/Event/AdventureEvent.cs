using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;

public interface IAdventureEvent
{
    int Rarity { get; }
    void Init(IInlineInvokeProxy CPH, SettingsManager.Scope scope);
    bool CanRun(Adventure.Details details);
    void Run(IInlineInvokeProxy CPH, Adventure.Details details);
}
