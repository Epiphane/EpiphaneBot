using System;
using System.Collections.Generic;
using System.Diagnostics.Tracing;
using System.Reflection;
using SpotifyAPI.Web;
using System.Threading.Tasks;

public partial class CPHInline
{
    private SettingsManager _settingsManager;
    private EventManager _eventManager;
    private SceneManager _sceneManager;
    private ExternalManager _externalManager;
    private TwitchManager _twitchManager;
    private RPGManager _rpgManager;
    private MusicManager _musicManager;
    private StreamManager _streamManager;

    public void Init()
    {
        if (_settingsManager == null)
        {
            _settingsManager = new SettingsManager();
        }

        if (_eventManager == null)
        {
            _eventManager = new EventManager(CPH);
        }

        if (_externalManager == null)
        {
            _externalManager = new ExternalManager(CPH);
        }

        if (_sceneManager == null)
        {
            _sceneManager = new SceneManager(CPH, _externalManager);
        }

        if (_twitchManager == null)
        {
            _twitchManager = new TwitchManager();
        }

        if (_streamManager == null)
        {
            _streamManager = new StreamManager(CPH);
        }

        if (_rpgManager == null)
        {
            _rpgManager = new RPGManager(CPH);
        }

        if (_musicManager == null)
        {
            _musicManager = new MusicManager(CPH, _settingsManager, _externalManager);
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

        _eventManager.OnFollow(user);

        return true;
    }

    public bool Action_Subscribe()
    {
        PrintArgs();
        string user = args["user"].ToString();

        _eventManager.OnSubscribe(user);

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

        _sceneManager.TransitionToScene(scenum);
        return true;
    }

    public bool Show_Browser()
    {
        _sceneManager.ShowBrowser();
        return true;
    }

    public bool Hide_Browser()
    {
        _sceneManager.HideBrowser();
        return true;
    }

    public bool Show_Timer()
    {
        _sceneManager.ShowTimer();
        return true;
    }

    public bool Hide_Timer()
    {
        _sceneManager.HideTimer(false);
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

        _sceneManager.SetMusicVisibility(visibility);
        return true;
    }

    public bool DEBUG_ToggleSimulateChat()
    {
        _sceneManager.DEBUG_ToggleSimulateChat();
        return true;
    }

    public bool ResetStreamState()
    {
        _streamManager.ResetState();
        return true;
    }

    public bool ClaimFirst()
    {
        int place;
        string user = args["user"].ToString();
        if (_streamManager.ClaimFirst(user, out place))
        {
            List<string> messages = new List<string> { "first", "second", "third" };
            List<string> endings = new List<string> { "2 prizes remaining", "1 prize remaining", "" };
            string message = messages[place];
            CPH.SendMessage($"You did it, {user}! You were {message} to claim the prize today! {endings[place]}");
        }
        else if (_streamManager.HasClaimed(user))
        {
            CPH.SendMessage($"You've already used that today, {user}. Nice try ;)");
        }
        else
        {
            
            CPH.SendMessage($"Sorry {user}, you were beaten by {_streamManager.Claimants} today. Better luck next time!");
        }

        return true;
    }

    public bool PlayMusic()
    {
        FullTrack track = _musicManager.StartPlaylist().GetAwaiter().GetResult();
        CPH.LogDebug($"Now playing: {track.Name}");
        return true;
    }

    public async void StartStreamSequence(bool goLive)
    {
        FullTrack track = _musicManager.StartPlaylist().GetAwaiter().GetResult();
        /*
        _sceneManager.TransitionToScene(SceneManager.FauxScene.Starting);
        _sceneManager.SetIntroRemainingTime(track);
        if (goLive)
        {
            CPH.ObsStartStreaming();
            CPH.ObsStartRecording();
        }
        await Task.Delay(track.DurationMs - 2000);
        _sceneManager.TransitionToScene(SceneManager.FauxScene.Speedrunning);
        */

    }

    public bool Test()
    {
        StartStreamSequence(false);

        return true;
    }

    public bool StartStream()
    {
        bool goLive = false;
        if (args.ContainsKey("goLive"))
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
        CPH.SendMessage($"Playlist: https://open.spotify.com/playlist/{_musicManager.Playlist}");
        return true;
    }

    public bool ShowCurrentSong()
    {
        FullTrack track = _musicManager.CurrentlyPlaying;
        if (track is null)
        {
            return false;
        }

        CPH.SendMessage($"Currently playing: {track.Name} by {_musicManager.CurrentArtist.Name}");
        return true;
    }

    public bool VoteOnSong()
    {
        Int64 vote = 0;
        if (args.ContainsKey("vote"))
        {
            vote = (Int64)args["vote"];
        }

        FullTrack track = _musicManager.CurrentlyPlaying;
        if (track is null)
        {
            return false;
        }

        CPH.LogDebug($"Voting {vote} on {track.Name}");
        //{"AccessToken":"BQArroV4I4pTLSWUJJUpB6KL4cafRrqb5Z-bPATlpQaxFGSOnpeIrsZ60WWWHxcyhR4cQGvB63cVDgAURkIBZe4DJVjkjJ4bqDi1LyTYewTLAmzKdMfaNDOr8eh3XsrZ48iax5CO_VzqXiHlapCzAweMMpCcLnF1jZuxUa50SxoxGZmpUctN13vhXsbDQHTi9g_Mvf1FFL93CvKUvwXGxeNRrZpG8Q4ec7emjtSBylScHDBb_X7f5nNYA0hf18RI1gUDV439BgGJkJD-9ahZM8vTDSWILiPUk1hQWapwclsn0vMQwu4g_vF02eS1rY0h","RefreshToken":"AQBjXx1E0vPpc_VhBoyRRDXoUlMKLr2hQgoDMIcdIJnRWyH63uH9YEZ5F6poPm6ccnYQyjZOzNzazCT8FhzJZXvFYflD578r-xbE4YZV9oNmcYSt093oGPh7kD8UrA4HLRo"}
        string key = $"Music_Score_{track.Name}";
        Int64 currentScore = (_settingsManager[key] is Int64) ? (Int64)_settingsManager[key] : 0;
        _settingsManager[key] = currentScore + vote;

        switch (vote)
        {
            case 1:
                CPH.SendMessage($"Yo, {track.Name} is a BANGER song! Let's gooooo");
                break;
            case -1:
                CPH.SendMessage($"I agree, {track.Name} SUCKS!");
                break;
        }

        return true;
    }
}
