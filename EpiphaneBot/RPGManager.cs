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

    private Setting<bool> DebugSetting;

    public RPGManager(IInlineInvokeProxy CPH, SettingsManager.Scope settings, string ConnectionString = null)
    {
        this.CPH = CPH;
        Settings = settings;

        DebugSetting = Settings.At("LogSQLCommands", true);

        var builder = DatabaseConfiguration.Build()
            .UsingProviderName("sqlite")
            .UsingCommandExecuting(OnExecutingCommand)
            .UsingConnectionString(ConnectionString ?? DefaultConnectionString);
        DB = builder.Create();

        User.CreateTable(DB);
        Adventure.InitEvents(CPH, DB, Settings.GetScope("Raid"));

        FirstEvent = new FirstEvent(DB, CPH, false);
    }

    public void OnExecutingCommand(object sender, DbCommandEventArgs cmd)
    {
        if (!DebugSetting)
        {
            return;
        }

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

    public bool InitRaid(long userId, string name, string[] message)
    {
        if (CurrentRaid != null)
        {
            throw new Exception("A raid is already going on!");
        }

        RaidEvent = Adventure.Create(CPH, DB, Settings.GetScope("Raid"), GetUser(userId, name), message);
        return RaidEvent != null;
    }

    public bool StartRaid(long userId, string name, string[] message)
    {
        if (InitRaid(userId, name, message))
        {
            CurrentRaid.Run();
        }
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

    public void ClearRaid()
    {
        RaidEvent = null;
    }

    public bool GiveCaterium(long userId, string name, string[] message)
    {
        if (CurrentRaid != null && CurrentRaid.state == Adventure.State.Running)
        {
            CPH.SendMessage("Please wait until the raid is over to gift caterium! epiphaPro1");
            return false;
        }

        User user = GetUser(userId, name);
        if (user == null)
        {
            return false;
        }

        if (message.Length < 2 || message[1] == "")
        {
            CPH.SendMessage("Usage: !give <other user> <amount>");
            return true;
        }

        string otherPerson = message[0];
        if (otherPerson[0] == '@' && otherPerson.Length > 1)
        {
            otherPerson = otherPerson.Substring(1);
        }

        User other = User.Find(CPH, DB, otherPerson);
        if (other == null)
        {
            CPH.SendMessage($"I couldn't find {otherPerson} :( maybe they haven't participated in a raid yet?");
            return false;
        }

        if (other.Id == user.Id)
        {
            CPH.SendMessage($"Nice try... epiphaPro1");
        }

        if (!int.TryParse(message[1], out int amount))
        {
            CPH.SendMessage($"Invalid gift amount: ${message}. !give <other user> <amount>");
            return false;
        }

        if (user.Caterium < amount)
        {
            CPH.SendMessage($"You don't have enough Caterium! (Current: {user.Caterium})");
            return false;
        }
        else if (amount < 1)
        {
            CPH.SendMessage($"That's rude");
            return false;
        }

        other.Caterium += amount;
        user.Caterium -= amount;

        CPH.SendMessage($"{user} gives {amount} caterium to {other}!");
        if (other.Caterium < 0)
        {
            other.Caterium = 0;
            other.Prestige++;
            if (other.Prestige > 1)
            {
                CPH.SendMessage($"{other} has earned 2 billion caterium and earned a golden nut trophy, for a total of {other.Prestige} trophies! Congratulations epiphaLuck1");
            }
            else
            {
                CPH.SendMessage($"{other} has earned 2 billion caterium and earned their first golden nut trophy! Congratulations epiphaLuck1");
            }
        }
        DB.Save(other);
        DB.Save(user);

        return true;
    }

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
