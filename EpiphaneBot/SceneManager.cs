﻿using System;
using Streamer.bot.Plugin.Interface;
using SpotifyAPI.Web;
using System.Collections.Generic;

public class SceneManager
{
    public enum SceneColor
    {
        Blue,
        Purple,
        Orange,
        Green,
    };

    private const string ChatBoxUrl = 
        "https://streamlabs.com/widgets/chat-box/v1/72D0E584F339D4D29FA65ECA62FCC7" +
        "558BC344C09A4A1C40AC10FD4A2A9BABB98106AB74ED2C05F99294EB48C54A7658594665F" +
        "8850DF3AA0089A091AE4B83E7AA2FF010177C5F639C580350534E25A39A1998F594D2451B" +
        "A17EDE237A5C4E68C64C8ACE3E2AFFD59833D4B347112B2875B359EC8C5A94CBA8D97646D7";

    private IInlineInvokeProxy CPH;
    private ExternalManager _externalManager;

    public SceneManager(IInlineInvokeProxy CPH, ExternalManager externalManager)
    {
        this.CPH = CPH;
        _externalManager = externalManager;

        _externalManager.OnProcessState("LiveSplit.exe", OnLiveSplit);
    }

    ~SceneManager()
    {
        _externalManager.OffProcessState("LiveSplit.exe", OnLiveSplit);
    }

    private void OnLiveSplit(string processName, bool running)
    {
        if (running)
        {
            ShowTimer();
        }
        else
        {
            HideTimer(true);
        }
    }

    public enum FauxScene
    {
        Starting,
        Ending,
        Speedrunning,
        SpeedrunningLarge,
        Browser,
    };

    public void TransitionToScene(FauxScene scene)
    {
        if (scene == FauxScene.Starting)
        {
            CPH.ObsMediaPlay("[SRC] Starting Soon", "Intro");
            ShowIntro();
        }
        else
        {
            HideIntro();
        }

        if (scene == FauxScene.Ending)
        {
            ShowOutro();
        }
        else
        {
            HideOutro();
        }

        if (scene == FauxScene.Speedrunning)
        {
            CPH.ObsShowFilter("Timer Black BG", "Fade Out");
            CPH.ObsShowFilter("[SRC] Browser", "Move Offscreen");
            CPH.ObsShowFilter("[SRC] Main Content", "Move Top Right");
            CPH.ObsShowFilter("[SRC] Music", "Move Bottom Right");
            CPH.ObsShowFilter("[SRC] Rewards Panel", "Move Bottom Left");
        }
        else if (scene == FauxScene.SpeedrunningLarge)
        {
            CPH.ObsShowFilter("Timer Black BG", "Fade In");
            CPH.ObsShowFilter("[SRC] Main Content", "Move Large");
            CPH.ObsShowFilter("[SRC] Music", "Move Offscreen");
            CPH.ObsShowFilter("[SRC] Rewards Panel", "Move Offscreen");
        }
    }

    public void ShowIntro()
    {
        CPH.ObsSourceMute("[A] Microphone", "Microphone");
        CPH.ObsSourceMute("[A] No Discord No Chrome", "No Discord No Chrome");
        CPH.ObsShowFilter("[SRC] Starting Soon", "Show");
    }

    public void HideIntro()
    {
        CPH.ObsShowFilter("[SRC] Starting Soon", "Fade Out");
        CPH.ObsSourceUnMute("[A] Microphone", "Microphone");
        CPH.ObsSourceUnMute("[A] No Discord No Chrome", "No Discord No Chrome");
    }

    public void ShowOutro()
    {
        CPH.ObsShowFilter("[SRC] Stream Ending", "Fade In");
    }

    public void HideOutro()
    {
        CPH.ObsShowFilter("[SRC] Stream Ending", "Fade Out");
    }

    public void SetIntroRemainingTime(FullTrack track)
    {
        int time = 5 * 60 - (track.DurationMs / 1000 - 5);
        CPH.ObsSetBrowserSource("[SRC] Starting Soon", "Intro Page", @"file://E:/Stream/starting.html?t=" + time);
    }

    public enum Visibility
    {
        Opaque,
        Half,
        Invisible,
    };

    public void SetMusicVisibility(Visibility vis)
    {
        switch (vis)
        {
            case Visibility.Opaque:
                CPH.ObsShowFilter("[SRC] Music", "Fade In");
                break;
            case Visibility.Half:
                CPH.ObsShowFilter("[SRC] Music", "Fade Half");
                break;
            case Visibility.Invisible:
                CPH.ObsShowFilter("[SRC] Music", "Fade Out");
                break;
        }
    }

    public void ShowBrowser()
    {
        CPH.ObsShowFilter("Timer Black BG", "Fade In");
        CPH.ObsShowFilter("[SRC] Browser", "Move Top Left");
    }

    public void HideBrowser()
    {
        CPH.ObsShowFilter("Timer Black BG", "Fade Out");
        CPH.ObsShowFilter("[SRC] Browser", "Move Offscreen");
    }

    public void ShowTimer()
    {
        CPH.ObsShowFilter("[E] Timer", "Fade In");
    }

    public void HideTimer(bool fast)
    {
        string suffix = fast ? " (Fast)" : "";
        CPH.ObsShowFilter("[E] Timer", $"Fade Out{suffix}");
    }

    public void RefreshChat()
    {
        CPH.ObsSetBrowserSource("[SRC] Chat", "Chat", $"{ChatBoxUrl}?{DateTime.Now}");
    }

    public void SetColor(SceneColor color)
    {
        string colorName = Enum.GetName(typeof(SceneColor), color);
        List<string> ColoredElements = new List<string>() { "[SRC] Backdrop", "[E] Music", "[SRC] Main Content" };
        foreach (string element in ColoredElements)
        {
            CPH.ObsShowFilter(element, $"Turn {colorName}");
        }

        foreach (string name in Enum.GetNames(typeof(SceneColor)))
        {
            if (name == colorName)
            {
                CPH.ObsShowFilter($"[E] Backdrop {name}", $"Fade In");
            }
            else
            {
                CPH.ObsShowFilter($"[E] Backdrop {name}", $"Fade Out");
            }
        }
    }

    //
    // CHAT SIMULATION
    //
    public void DEBUG_SimulateChat()
    {
        CPH.ObsSetSourceVisibility("[SRC] Chat", "Chat Simulate", true);
        CPH.ObsSetSourceVisibility("[SRC] Chat", "Chat", false);
    }

    public void DEBUG_UnsimulateChat()
    {
        CPH.ObsSetSourceVisibility("[SRC] Chat", "Chat Simulate", false);
        CPH.ObsSetSourceVisibility("[SRC] Chat", "Chat", true);
    }

    public void DEBUG_ToggleSimulateChat()
    {
        if (CPH.ObsIsSourceVisible("[SRC] Chat", "Chat"))
        {
            DEBUG_SimulateChat();
        }
        else
        {
            DEBUG_UnsimulateChat();
        }
    }
};
