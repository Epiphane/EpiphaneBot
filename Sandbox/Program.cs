using System;

using TwitchLib.Api;
using TwitchLib.Api.Services;
using TwitchLib.Api.Services.Events;
using TwitchLib.Api.Services.Events.LiveStreamMonitor;
using TwitchLib.Api.Helix.Models.Users.GetUserFollows;
using System.Threading.Tasks;
using SpotifyAPI.Web;
using System.Text.Json;
using System.IO;
using PetaPoco;
using PetaPoco.Providers;
using System.Collections.Generic;

class Program
{
    private Dictionary<string, object> args = new Dictionary<string, object>();

    private T GetArg<T>(string name)
    {
        if (!args.ContainsKey(name))
        {
            return default(T);
        }

        object value = args[name];
        if (typeof(T) != typeof(string) && value is string)
        {
            string str = (string)value;
            if (typeof(T) == typeof(int))
            {
                return (T)(object)int.Parse(str);
            }
            if (typeof(T) == typeof(long))
            {
                return (T)(object)long.Parse(str);
            }
        }

        return (T)value;
    }

    void Run()
    {
        args["n64"] = (long)10;
        args["n32"] = (int)11;
        args["str"] = "string";
        args["num"] = "12";

        long n64 = GetArg<long>("n64");
        int n32 = GetArg<int>("n32");
        string str = GetArg<string>("str");
        long num = GetArg<Int64>("num");
    }

    static void Main()
    {
        new Program().Run();
    }
}
