using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Management;
using System.Diagnostics;
using SpotifyAPI.Web;
using Streamer.bot.Plugin.Interface;

public class MusicManager
{
    public string Playlist
    {
        get;
        private set;
    } = "1PivgIF1ZykwHu1pC8a4X5";

    private const string kAccessToken = "Music_AccessToken";
    private const string kRefreshToken = "Music_RefreshToken";
    private const string kExpirationTime = "Music_ExpirationTime";
    private const string kComputerName = "Thomas-PC";
    private const int kAppCommand = 0x0319;

    public enum Command
    {
        CMD_NONE = 0,
        CMD_PLAYPAUSE = 917504,
        CMD_MUTE = 524288,
        CMD_VOLUMEDOWN = 589824,
        CMD_VOLUMEUP = 655360,
        CMD_STOP = 851968,
        CMD_PREVIOUS = 786432,
        CMD_NEXT = 720896,
    };

    [DllImport("user32.dll")]
    public static extern int SendMessage(IntPtr hWnd, int wMsg, IntPtr wParam, IntPtr lParam);

    private IntPtr SpotifyWindow = IntPtr.Zero;

    // Inputs
    private IInlineInvokeProxy CPH;
    private SettingsManager _settingsManager;
    private ExternalManager _externalManager;

    // API Client
    private SpotifyClient Spotify;
    private DateTime ExpirationTime;

    public MusicManager(IInlineInvokeProxy CPH, SettingsManager settingsManager, ExternalManager externalManager)
    {
        this.CPH = CPH;
        _settingsManager = settingsManager;
        _externalManager = externalManager;

        _externalManager.OnProcessState("Spotify.exe", OnSpotify);
        SpotifyWindow = GetSpotifyWindow();

        if (_settingsManager.Has(kAccessToken) && _settingsManager.Has(kExpirationTime))
        {
            Spotify = new SpotifyClient((string)_settingsManager[kAccessToken]);
            ExpirationTime = new DateTime((long)_settingsManager[kExpirationTime]);
            CheckToken();
        }
        else
        {
            RefreshToken("No access token found");
        }

    }

    ~MusicManager()
    {
        _externalManager.OffProcessState("Spotify.exe", OnSpotify);
    }

    private void CheckToken()
    {
        if (DateTime.UtcNow < ExpirationTime)
        {
            return;
        }

        RefreshToken("Spotify access token expired");
    }

    private void RefreshToken(string reason)
    {
        CPH.LogDebug($"{reason}, refreshing token...");
        var newResponse = new OAuthClient().RequestToken(
            new AuthorizationCodeRefreshRequest("2b77d444e13e48cfb2effe8143831ae6", "f92bd400e91745f6a27ead8d4df3376e", (string)_settingsManager["Music_RefreshToken"])
        ).Result;

        ExpirationTime = newResponse.CreatedAt.AddSeconds(newResponse.ExpiresIn);
        Spotify = new SpotifyClient(newResponse.AccessToken);
        _settingsManager["Music_AccessToken"] = newResponse.AccessToken;
        _settingsManager["Music_Expiration"] = ExpirationTime.Ticks;
    }

    public FullTrack CurrentlyPlaying
    {
        get
        {
            CurrentlyPlaying playing = Spotify.Player.GetCurrentlyPlaying(new PlayerCurrentlyPlayingRequest()).Result;
            if (playing == null)
            {
                return null;
            }

            if (!(playing.Item is FullTrack))
            {
                return null;
            }

            return (FullTrack)playing.Item;
        }
    }

    public SimpleArtist CurrentArtist
    {
        get
        {
            if (CurrentlyPlaying == null)
            {
                return null;
            }

            List<SimpleArtist> artists = CurrentlyPlaying.Artists;
            if (artists.Count == 0)
            {
                return null;
            }

            return artists[0];
        }
    }

    private IntPtr GetSpotifyWindow()
    {
        Process[] processes = Process.GetProcessesByName("Spotify");
        foreach (Process p in processes)
        {
            if (p.MainWindowHandle != IntPtr.Zero)
            {
                return p.MainWindowHandle;
            }
        }

        return IntPtr.Zero;
    }

    private void OnSpotify(string processName, bool running)
    {
        // Confirm we're holding onto the right spotify window
        CPH.LogDebug("Spotify process state updated, refreshing window handle");
        SpotifyWindow = GetSpotifyWindow();
    }

    private void SendCommand(Command command)
    {
        SendMessage(SpotifyWindow, kAppCommand, IntPtr.Zero, (IntPtr)command);
    }

    public Device PrimaryDevice
    {
        get
        {
            DeviceResponse devices = Spotify.Player.GetAvailableDevices().Result;
            if (devices == null || devices.Devices == null || devices.Devices.Count == 0)
            {
                return null;
            }

            Device pc = devices.Devices.Find(d => d.Name == kComputerName);
            if (pc == null)
            {
                return devices.Devices.First();
            }

            return pc;
        }
    }

    public async Task<FullTrack> StartPlaylist(int? offset = null)
    {
        Device primary = PrimaryDevice;
        await Spotify.Player.SetShuffle(new PlayerShuffleRequest(true) { DeviceId = primary.Id });
        Paging<PlaylistTrack<IPlayableItem>> tracks = await Spotify.Playlists.GetItems(Playlist);
        if (offset == null)
        {
            offset = new Random().Next(tracks.Total ?? 0);
        }
        await Spotify.Player.ResumePlayback(new PlayerResumePlaybackRequest()
        {
            DeviceId = primary.Id,
            OffsetParam = new PlayerResumePlaybackRequest.Offset()
            {
                Position = offset
            },
            ContextUri = $"spotify:playlist:{Playlist}"
        });
        Paging<PlaylistTrack<IPlayableItem>> nextSong = await Spotify.Playlists.GetItems(Playlist, new PlaylistGetItemsRequest()
        {
            Offset = offset,
            Limit = 1,
        });
        return (FullTrack)nextSong.Items.First().Track;
    }

    public void Play()
    {
        Spotify.Player.ResumePlayback();
    }

    public void Stop()
    {
        Spotify.Player.PausePlayback();
    }

    public void Previous()
    {
        SendCommand(Command.CMD_PREVIOUS);
    }

    public void Next()
    {
        SendCommand(Command.CMD_NEXT);
    }

    public void Mute()
    {
        SendCommand(Command.CMD_MUTE);
    }

    public void VolumeDown()
    {
        SendCommand(Command.CMD_VOLUMEDOWN);
    }

    public void VolumeUp()
    {
        SendCommand(Command.CMD_VOLUMEUP);
    }
}
