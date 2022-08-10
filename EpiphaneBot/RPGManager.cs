using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;
using PetaPoco;
using PetaPoco.Providers;

public class RPGManager
{
    private readonly string ConnectionString = $@"URI=file:{SettingsManager.SettingsPath}/RPG.db";

    public enum Resource
    {
        Wood,
        Stone,
        Iron,
        Rubies,
        Gold,
    };

    private readonly IInlineInvokeProxy CPH;
    private readonly IDatabase DB;

    private readonly List<RPGEvent> Events = new List<RPGEvent>();

    public RPGManager(IInlineInvokeProxy CPH)
    {
        this.CPH = CPH;

        var builder = DatabaseConfiguration.Build()
            .UsingProviderName("sqlite")
            .UsingCommandExecuting(OnExecutingCommand)
            .UsingConnectionString(ConnectionString);
        DB = builder.Create();

        StartEvent((_, _2) => new FirstEvent(DB, CPH));
    }

    public void OnExecutingCommand(object sender, DbCommandEventArgs cmd)
    {
        CPH.LogInfo(cmd.Command.CommandText);
        foreach (System.Data.SQLite.SQLiteParameter sqlParam in cmd.Command.Parameters)
        {
            CPH.LogInfo(string.Format("Name: {0}; Value: {1}", sqlParam.ParameterName, sqlParam.Value));
        }
    }

    public void AddResources(long userId, string name, Resource resource, int amount)
    {
        CPH.LogDebug($"Adding resources: {userId}, {name}, {resource}, {amount}");
        User user = User.Get(CPH, DB, userId, name);
        switch (resource)
        {
            case Resource.Wood:
                user.Wood += amount;
                break;
            case Resource.Stone:
                user.Stone += amount;
                break;
            case Resource.Iron:
                user.Iron += amount;
                break;
            case Resource.Rubies:
                user.Rubies += amount;
                break;
            case Resource.Gold:
                user.Gold += amount;
                break;
            default:
                throw new ArgumentException($"Invalid resource: {Enum.GetName(typeof(Resource), resource)}");
        }

        DB.Save(user);
    }

    public bool StartEvent<E>(Func<IDatabase, IInlineInvokeProxy, E> factory) where E : RPGEvent
    {
        E ev = factory(DB, CPH);

        Events.Add(ev);
        ev.Start();

        return true;
    }

    public void DeleteEventsOfType<E>() where E : RPGEvent
    {
        Events.RemoveAll(e => e is E);
    }

    public bool HandleCommand(long userId, string name, string message)
    {
        User user = User.Get(CPH, DB, userId, name);
        
        for (int i = Events.Count - 1; i >= 0; --i)
        {
            if (Events[i].Handle(user, message))
            {
                return true;
            }
        }

        return false;
    }
}
