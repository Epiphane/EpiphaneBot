using System;
using System.Collections.Generic;
using System.Diagnostics.Tracing;
using System.Reflection;
using SpotifyAPI.Web;
using System.Threading.Tasks;
using System.Threading;

public partial class CPHInline
{
    private CPHProxy CPHProxy;

    private SettingsManager settingsManager;
    private EventManager eventManager;
    private SceneManager sceneManager;
    private ExternalManager externalManager;
    private TwitchManager twitchManager;
    private RPGManager rpgManager;
    private MusicManager musicManager;
    private StreamManager streamManager;

    public void Init()
    {
        if (CPHProxy == null)
        {
            CPHProxy = new CPHProxy(proxy: CPH)
            {
                Live = true
            };
        }

        if (settingsManager == null)
        {
            settingsManager = new SettingsManager(CPHProxy);
        }

        if (eventManager == null)
        {
            eventManager = new EventManager(CPHProxy);
        }

        if (externalManager == null)
        {
            externalManager = new ExternalManager(CPHProxy);
        }

        if (sceneManager == null)
        {
            sceneManager = new SceneManager(CPHProxy, externalManager);
        }

        if (twitchManager == null)
        {
            twitchManager = new TwitchManager();
        }

        if (streamManager == null)
        {
            streamManager = new StreamManager(CPHProxy);
        }

        if (rpgManager == null)
        {
            rpgManager = new RPGManager(CPHProxy, settingsManager["RPG"]);
        }

        if (musicManager == null)
        {
            musicManager = new MusicManager(CPHProxy, settingsManager["Music"], settingsManager["MusicScores"]);
        }

        Show_Timer();
    }

    public bool PrintArgs()
    {
        foreach (var arg in args)
        {
            CPH.LogInfo($"LogVars :: {arg.Key} = {arg.Value}");
        }

        return true;
    }

    public bool SetTestingMode()
    {
        CPHProxy.Live = GetArg<bool>("live");
        return true;
    }

    // *-----------------------------------------+------------------------------------------*
    // |                                                                                    |
    // |                                   Stream Events                                    |
    // |                                                                                    |
    // *-----------------------------------------+------------------------------------------*
    public bool Action_Follow()
    {
        PrintArgs();
        string user = args["user"].ToString();

        eventManager.OnFollow(user);

        return true;
    }

    public bool Action_Subscribe()
    {
        PrintArgs();
        string user = args["user"].ToString();

        eventManager.OnSubscribe(user);

        return true;
    }

    public bool Perform_Transition()
    {
        CPHProxy.LogDebug("Transition!");
        SceneManager.FauxScene scenum;
        string transitionName = args["transitionName"].ToString();
        if (!Enum.TryParse(transitionName, out scenum))
        {
            CPHProxy.LogWarn($"Transition name not recognized: {transitionName}");
            return false;
        }

        sceneManager.TransitionToScene(scenum);
        return true;
    }

    // *-----------------------------------------+------------------------------------------*
    // |                                                                                    |
    // |                                       Debug                                        |
    // |                                                                                    |
    // *-----------------------------------------+------------------------------------------*
    public bool Test()
    {
        SettingsManager.Scope scope = settingsManager.GetScope("Test");
        Setting<string> val = scope.At("Test1", "Test value");
        CPHProxy.LogDebug(val);

        return true;
    }

    public bool TestAsync()
    {
        CPHProxy.LogDebug("TestAsync starting");
        CPHProxy.RunAction("Test", false);
        CPHProxy.LogDebug("TestAsync ending");
        return true;
    }

    public bool DEBUG_ToggleSimulateChat()
    {
        sceneManager.DEBUG_ToggleSimulateChat();
        return true;
    }

    // *-----------------------------------------+------------------------------------------*
    // |                                                                                    |
    // |                                Core Stream Actions                                 |
    // |                                                                                    |
    // *-----------------------------------------+------------------------------------------*
    public bool StartStream()
    {
        if (CPHProxy.ObsIsStreaming())
        {
            return false;
        }

        bool goLive = false;
        {
            if (args["goLive"] is bool && (bool)args["goLive"] == true)
            {
                goLive = true;
            }
        }
        Task task = StartStreamSequence(goLive);

        return true;
    }

    public bool Show_Browser()
    {
        sceneManager.ShowBrowser();
        return true;
    }

    public bool Hide_Browser()
    {
        sceneManager.HideBrowser();
        return true;
    }

    public bool Show_Timer()
    {
        sceneManager.ShowTimer();
        TimerVisible = true;
        return true;
    }

    public bool Hide_Timer()
    {
        sceneManager.HideTimer(false);
        TimerVisible = false;
        return true;
    }

    private bool TimerVisible = true;
    public bool Toggle_Timer()
    {
        if (TimerVisible)
        {
            Hide_Timer();
        }
        else
        {
            Show_Timer();
        }
        return true;
    }

    public bool SetMusicVisibility()
    {
        SceneManager.Visibility visibility;
        string arg = args["visibility"].ToString();
        if (!Enum.TryParse(arg, out visibility))
        {
            CPHProxy.LogWarn($"Visibility not recognized: {arg}");
            return false;
        }

        sceneManager.SetMusicVisibility(visibility);
        return true;
    }

    public bool SetColor()
    {
        if (Enum.TryParse<SceneManager.SceneColor>(GetArg<string>("color"), out SceneManager.SceneColor color))
        {
            sceneManager.SetColor(color);
            return true;
        }

        return false;
    }

    // *-----------------------------------------+------------------------------------------*
    // |                                                                                    |
    // |                                      Helpers                                       |
    // |                                                                                    |
    // *-----------------------------------------+------------------------------------------*
    private T GetArg<T>(string name, bool optional = false)
    {
        if (!args.ContainsKey(name))
        {
            if (optional)
            {
                return default;
            }
            throw new Exception($"Argument {name} not provided");
        }

        object value = args[name];
        if (typeof(T) != typeof(string) && value is string)
        {
            string str = (string)value;
            if (typeof(T) == typeof(int))
            {
                return (T)(object)int.Parse(str);
            }
            else if (typeof(T) == typeof(long))
            {
                return (T)(object)long.Parse(str);
            }
            else
            {
                if (optional)
                {
                    return default;
                }
                throw new Exception($"Parsing {typeof(T)} from a string is not supported");
            }
        }

        if (!(value is T))
        {
            CPHProxy.LogWarn($"Argument {name} ({name.GetType()}) is not of type {typeof(T)}.");
        }

        return (T)value;
    }

    private long UserId { get { return GetArg<long>("userId"); } }
    private string UserName { get { return GetArg<string>("user"); } }
    private string Command { get { return GetArg<string>("command"); } }
    private string[] RawInput { get { return (GetArg<string>("rawInput", true) ?? "").Split(new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries); } }

    public bool GetSetting()
    {
        string path = GetArg<string>("path");
        string[] parts = path.Split(new[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

        if (parts.Length == 0)
        {
            CPH.LogWarn("Path is empty");
            return false;
        }

        SettingsManager.Scope scope = settingsManager.GetScope(parts[0]);
        for (int i = 1; i < parts.Length - 1; i++)
        {
            scope = scope.GetScope(parts[i], false);
        }

        CPH.LogInfo($"Setting {path}: {scope.GetValue(parts[parts.Length - 1])}");

        return true;
    }

    public bool ModifySetting()
    {
        string path = GetArg<string>("path");
        string value = GetArg<string>("value");
        string[] parts = path.Split(new[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

        if (parts.Length == 0)
        {
            CPH.LogWarn("Path is empty");
            return false;
        }

        SettingsManager.Scope scope = settingsManager.GetScope(parts[0]);
        for (int i = 1; i < parts.Length - 1; i++)
        {
            scope = scope.GetScope(parts[i], false);
        }

        scope.SetValue(parts[parts.Length - 1], value);

        return true;
    }

    public bool ReloadSettings()
    {
        settingsManager.LoadSettings();
        return true;
    }

    public bool PlayMusic()
    {
        FullTrack track = musicManager.StartPlaylist().GetAwaiter().GetResult();
        CPHProxy.LogDebug($"Now playing: {track.Name}");
        return true;
    }

    private int StreamSequenceId = 0;
    public async Task StartStreamSequence(bool goLive)
    {
        int id = ++StreamSequenceId;

        // Start the music
        FullTrack track = musicManager.StartPlaylist().GetAwaiter().GetResult();

        // Go to the "Stream Starting" screen
        sceneManager.TransitionToScene(SceneManager.FauxScene.Starting);
        sceneManager.SetIntroRemainingTime(track);
        sceneManager.RefreshChat();
        rpgManager.ResetFirstEvent(true);
        if (goLive)
        {
            CPHProxy.ObsStartStreaming();
            CPHProxy.ObsStartRecording();
        }

        await Task.Delay(track.DurationMs - 2000);
        // Make sure it hasn't happened twice now
        if (id != StreamSequenceId)
        {
            return;
        }

        sceneManager.TransitionToScene(SceneManager.FauxScene.Speedrunning);
    }

    // *-----------------------------------------+------------------------------------------*
    // |                                                                                    |
    // |                                        RPG                                         |
    // |                                                                                    |
    // *-----------------------------------------+------------------------------------------*
    public bool SendFirstCommand()
    {
        return rpgManager.HandleFirstCommand(UserId, UserName, RawInput);
    }

    public bool StartRaid()
    {
        if (!CPH.ObsIsStreaming())
        {
            CPH.SendMessage("Nice try, but you can't start a raid off-stream ;)");
            return false;
        }

        if (rpgManager.CurrentRaid is null)
        {
            rpgManager.StartRaid(UserId, UserName, RawInput);
            return true;
        }

        // Raid already going on
        return false;
    }

    public bool SendRaidCommand()
    {
        return rpgManager.HandleRaidCommand(UserId, UserName, RawInput);
    }

    public bool GetGold()
    {
        User user = rpgManager.GetUser(UserId, UserName);
        if (user is null)
        {
            return false;
        }

        CPH.SendMessage($"{user}, you have {user.Caterium} caterium!");
        return true;
    }

    /*
    public bool GiveCaterium()
    {
    }
    */

    public bool BuyCaterium()
    {
        return rpgManager.BuyCaterium(UserId, UserName, 10);
    }

    public bool BuyLotsOfCaterium()
    {
        return rpgManager.BuyCaterium(UserId, UserName, 150);
    }

    // *-----------------------------------------+------------------------------------------*
    // |                                                                                    |
    // |                                       Music                                        |
    // |                                                                                    |
    // *-----------------------------------------+------------------------------------------*
    public bool ShowCurrentPlaylist()
    {
        CPHProxy.SendMessage($"Playlist: https://open.spotify.com/playlist/{musicManager.Playlist}");
        return true;
    }

    public bool ShowCurrentSong()
    {
        FullTrack track = musicManager.CurrentlyPlaying;
        if (track is null)
        {
            return false;
        }

        CPHProxy.SendMessage($"Currently playing: {track.Name} by {musicManager.CurrentArtist.Name}");
        return true;
    }

    public bool VoteOnSong()
    {
        return musicManager.VoteOnSong(UserName, GetArg<long>("vote"));
    }
}
