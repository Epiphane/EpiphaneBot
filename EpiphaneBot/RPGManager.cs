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
    private readonly string DefaultConnectionString = $@"URI=file:{SettingsManager.SettingsPath}/RPG.db";

    private readonly IInlineInvokeProxy CPH;
    private readonly SettingsManager.Scope Settings;
    private readonly IDatabase DB;

    private FirstEvent FirstEvent = null;
    private Adventure RaidEvent = null;

    public Adventure CurrentRaid
    {
        get
        {
            if (RaidEvent != null && RaidEvent.IsDone)
            {
                RaidEvent = null;
            }

            return RaidEvent;
        }
    }

    [Serializable]
    struct Dingo
    {
        public string name;
    }

    public RPGManager(IInlineInvokeProxy CPH, SettingsManager.Scope settings, string ConnectionString = null)
    {
        this.CPH = CPH;
        Settings = settings;

        var builder = DatabaseConfiguration.Build()
            .UsingProviderName("sqlite")
            .UsingCommandExecuting(OnExecutingCommand)
            .UsingConnectionString(ConnectionString ?? DefaultConnectionString);
        DB = builder.Create();

        User.CreateTable(DB);

        FirstEvent = new FirstEvent(DB, CPH, false);
    }

    public void OnExecutingCommand(object sender, DbCommandEventArgs cmd)
    {
        string command = cmd.Command.CommandText;
        foreach (System.Data.SQLite.SQLiteParameter sqlParam in cmd.Command.Parameters)
        {
            command = command.Replace(sqlParam.ParameterName, sqlParam.Value.ToString());
        }
        CPH.LogInfo($"RPGManager: Running {command}");
    }

    public User GetUser(long userId, string name)
    {
        return User.Get(CPH, DB, userId, name);
    }

    public void ResetFirstEvent(bool real)
    {
        FirstEvent = new FirstEvent(DB, CPH, real);
    }

    public bool HandleFirstCommand(long userId, string name, string[] message)
    {
        if (FirstEvent == null)
        {
            return false;
        }

        return FirstEvent.Handle(GetUser(userId, name), message);
    }

    public bool StartRaid(long userId, string name, string[] message)
    {
        if (CurrentRaid != null)
        {
            throw new Exception("A raid is already going on!");
        }

        RaidEvent = Adventure.Create(CPH, DB, Settings.GetScope("Raid"), GetUser(userId, name), message);
        RaidEvent.Run();
        return true;
    }

    public bool HandleRaidCommand(long userId, string name, string[] message)
    {
        User user = GetUser(userId, name);
        if (user == null)
        {
            return false;
        }

        if (CurrentRaid == null)
        {
            CPH.RunAction("Start Raid", false);
            return true;
        }

        return CurrentRaid.Handle(user, message);
    }

    /*
    public bool GiveCaterium(long userId, string name, string[] message)
    {
        User user = GetUser(userId, name);
        if (user == null)
        {
            return false;
        }

        if (message.Length < 2)
        {
            CPH.SendMessage("Usage: !give <other user> <amount>");
            return true;
        }

        string otherPerson = message[0];
        if (!int.TryParse(message[1], out int amount))
        {
            CPH.SendMessage($"Invalid investment: ${message}. Type !raid <number> to start a raid");
            return false;
        }
    }
    */

    public bool BuyCaterium(long userId, string name, int amount)
    {
        User user = GetUser(userId, name);
        if (user is null)
        {
            return false;
        }

        user.Caterium += amount;
        DB.Save(user);
        CPH.SendMessage($"{user}, you now have {user.Caterium} caterium!");
        return true;
    }
}
