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

    private const string kAccessToken = "AccessToken";
    private const string kRefreshToken = "RefreshToken";
    private const string kExpirationTime = "ExpirationTime";
    private const string kClientId = "ClientId";
    private const string kClientSecret = "ClientSecret";
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

    // Inputs
    private readonly IInlineInvokeProxy CPH;
    private readonly SettingsManager.Scope settings;

    // API Client
    private SpotifyClient Spotify;
    private DateTime ExpirationTime;

    public MusicManager(IInlineInvokeProxy CPH, SettingsManager.Scope settings)
    {
        this.CPH = CPH;
        this.settings = settings;

        if (settings.Has(kAccessToken) && settings.Has(kExpirationTime))
        {
            Spotify = new SpotifyClient((string)settings[kAccessToken]);
            ExpirationTime = new DateTime((long)settings[kExpirationTime]);
            CheckToken();
        }
        else
        {
            RefreshToken("No access token found");
        }

    }

    ~MusicManager()
    {
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
        CPH.LogInfo($"{reason}, refreshing token...");
        var newResponse = new OAuthClient().RequestToken(
            new AuthorizationCodeRefreshRequest((string)settings[kClientId], (string)settings[kClientSecret], (string)settings[kRefreshToken])
        ).Result;

        ExpirationTime = newResponse.CreatedAt.AddSeconds(newResponse.ExpiresIn);
        Spotify = new SpotifyClient(newResponse.AccessToken);
        settings[kAccessToken] = newResponse.AccessToken;
        settings[kExpirationTime] = ExpirationTime.Ticks;
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
    }

    private void SendCommand(Command command)
    {
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
