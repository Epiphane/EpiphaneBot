using System;
using System.Collections.Generic;
using System.Diagnostics.Tracing;
using System.Reflection;
using SpotifyAPI.Web;
using System.Threading.Tasks;

public partial class CPHInline
{
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
        if (settingsManager == null)
        {
            settingsManager = new SettingsManager(CPH);
        }

        if (eventManager == null)
        {
            eventManager = new EventManager(CPH);
        }

        if (externalManager == null)
        {
            externalManager = new ExternalManager(CPH);
        }

        if (sceneManager == null)
        {
            sceneManager = new SceneManager(CPH, externalManager);
        }

        if (twitchManager == null)
        {
            twitchManager = new TwitchManager();
        }

        if (streamManager == null)
        {
            streamManager = new StreamManager(CPH);
        }

        if (rpgManager == null)
        {
            rpgManager = new RPGManager(CPH);
        }

        if (musicManager == null)
        {
            musicManager = new MusicManager(CPH, settingsManager["Music"], settingsManager["MusicScores"]);
        }
    }

    public bool PrintArgs()
    {
        foreach (var arg in args)
        {
            CPH.LogInfo($"LogVars :: {arg.Key} = {arg.Value}");
        }

        return true;
    }

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
        SceneManager.FauxScene scenum;
        string transitionName = args["transitionName"].ToString();
        if (!Enum.TryParse(transitionName, out scenum))
        {
            CPH.LogWarn($"Transition name not recognized: {transitionName}");
            return false;
        }

        sceneManager.TransitionToScene(scenum);
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
        return true;
    }

    public bool Hide_Timer()
    {
        sceneManager.HideTimer(false);
        return true;
    }

    public bool SetMusicVisibility()
    {
        SceneManager.Visibility visibility;
        string arg = args["visibility"].ToString();
        if (!Enum.TryParse(arg, out visibility))
        {
            CPH.LogWarn($"Visibility not recognized: {arg}");
            return false;
        }

        sceneManager.SetMusicVisibility(visibility);
        return true;
    }

    public bool DEBUG_ToggleSimulateChat()
    {
        sceneManager.DEBUG_ToggleSimulateChat();
        return true;
    }

    public bool ResetStreamState()
    {
        streamManager.ResetState();
        return true;
    }

    public bool ClaimFirst()
    {
        int place;
        string user = args["user"].ToString();
        if (streamManager.ClaimFirst(user, out place))
        {
            List<string> messages = new List<string> { "first", "second", "third" };
            List<string> endings = new List<string> { "2 prizes remaining", "1 prize remaining", "" };
            string message = messages[place];
            CPH.SendMessage($"You did it, {user}! You were {message} to claim the prize today! {endings[place]}");
        }
        else if (streamManager.HasClaimed(user))
        {
            CPH.SendMessage($"You've already used that today, {user}. Nice try ;)");
        }
        else
        {

            CPH.SendMessage($"Sorry {user}, you were beaten by {streamManager.Claimants} today. Better luck next time!");
        }

        return true;
    }

    public bool PlayMusic()
    {
        FullTrack track = musicManager.StartPlaylist().GetAwaiter().GetResult();
        CPH.LogDebug($"Now playing: {track.Name}");
        return true;
    }

    private int StreamSequenceId = 0;

    public async void StartStreamSequence(bool goLive)
    {
        int id = ++StreamSequenceId;
        FullTrack track = musicManager.StartPlaylist().GetAwaiter().GetResult();
        sceneManager.TransitionToScene(SceneManager.FauxScene.Starting);
        sceneManager.SetIntroRemainingTime(track);
        sceneManager.RefreshChat();
        rpgManager.DeleteEventsOfType<FirstEvent>();
        rpgManager.StartEvent((DB, CPH) => new FirstEvent(DB, CPH));
        if (goLive)
        {
            CPH.ObsStartStreaming();
            CPH.ObsStartRecording();
        }

        await Task.Delay(track.DurationMs - 2000);
        // Make sure it hasn't happened twice now
        if (id != StreamSequenceId)
        {
            return;
        }

        sceneManager.TransitionToScene(SceneManager.FauxScene.Speedrunning);
    }

    public bool SendRPGCommand()
    {
        if (!args.ContainsKey("userId"))
        {
            CPH.LogWarn("SendRPGCommand called without a userId argument");
            return false;
        }

        if (!args.ContainsKey("user"))
        {
            CPH.LogWarn("SendRPGCommand called without a user argument");
            return false;
        }

        if (!args.ContainsKey("command"))
        {
            CPH.LogWarn("SendRPGCommand called without a message argument");
            return false;
        }

        long userId = args["userId"] is string ? int.Parse((string)args["userId"]) : (Int64)args["userId"];
        string user = (string)args["user"];
        string message = (string)args["command"];
        return rpgManager.HandleCommand(userId, user, message);
    }

    public bool Test()
    {
        PrintArgs();
        long userId = args["userId"] is string ? int.Parse((string)args["userId"]) : (Int64)args["userId"];
        rpgManager.AddResources(userId, (string)args["user"], RPGManager.Resource.Wood, 12);

        return true;
    }

    public bool StartStream()
    {
        if (CPH.ObsIsStreaming())
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
        StartStreamSequence(goLive);

        return true;
    }

    public bool ShowCurrentPlaylist()
    {
        CPH.SendMessage($"Playlist: https://open.spotify.com/playlist/{musicManager.Playlist}");
        return true;
    }

    public bool ShowCurrentSong()
    {
        FullTrack track = musicManager.CurrentlyPlaying;
        if (track is null)
        {
            return false;
        }

        CPH.SendMessage($"Currently playing: {track.Name} by {musicManager.CurrentArtist.Name}");
        return true;
    }

    public bool VoteOnSong()
    {
        if (!args.ContainsKey("user"))
        {
            CPH.LogWarn("VoteOnSong called without a user argument");
            return false;
        }

        if (!args.ContainsKey("vote"))
        {
            CPH.LogWarn("VoteOnSong called without a vote argument");
            return false;
        }

        Int64 vote = (Int64)args["vote"];
        string user = (string)args["user"];

        return musicManager.VoteOnSong(user, vote);
    }
}
