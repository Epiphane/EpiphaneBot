using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;

public abstract class RPGEvent
{
    protected readonly PetaPoco.IDatabase DB;
    protected readonly IInlineInvokeProxy CPH;

    protected RPGEvent(PetaPoco.IDatabase DB, IInlineInvokeProxy CPH)
    {
        this.DB = DB;
        this.CPH = CPH;
    }

    public abstract bool IsDone { get; }
    public abstract void Run();
    public abstract bool Handle(User user, string[] message);
}
