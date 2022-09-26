using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SpotifyAPI.Web;
using Streamer.bot.Plugin.Interface;

public class MusicManager
{
    public string Playlist { get; private set; } = "1PivgIF1ZykwHu1pC8a4X5";
    private const string kAccessToken = "AccessToken";
    private const string kRefreshToken = "RefreshToken";
    private const string kExpirationTime = "ExpirationTime";
    private const string kClientId = "ClientId";
    private const string kClientSecret = "ClientSecret";
    private const string kComputerName = "Thomas-PC";

    // Inputs
    private readonly IInlineInvokeProxy CPH;
    private readonly SettingsManager.Scope settings;
    private readonly SettingsManager.Scope scores;

    // API Client
    private SpotifyClient _Spotify;
    private SpotifyClient Spotify
    {
        get
        {
            CheckToken();
            return _Spotify;
        }
        set => _Spotify = value;
    }
    private DateTime ExpirationTime;

    [Serializable]
    private class SongEntry
    {
        public string Name;
        public string Artist;
        public int Score = 0;
        public HashSet<string> Upvotes;
        public HashSet<string> Downvotes;
    }

    public MusicManager(IInlineInvokeProxy CPH, SettingsManager.Scope settings, SettingsManager.Scope scores)
    {
        this.CPH = CPH;
        this.settings = settings;
        this.scores = scores;

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

    public Device PrimaryDevice
    {
        get
        {
            DeviceResponse devices = Spotify.Player.GetAvailableDevices().Result;
            CPH.LogInfo($"ND: {devices.Devices.Count}");
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
        CPH.LogInfo("Finding primary device...");
        Device primary = PrimaryDevice;
        CPH.LogInfo("Setting shuffle to true...");
        await Spotify.Player.SetShuffle(new PlayerShuffleRequest(true) { DeviceId = primary.Id });
        CPH.LogInfo("Getting list of tracks...");
        Paging<PlaylistTrack<IPlayableItem>> tracks = await Spotify.Playlists.GetItems(Playlist);
        if (offset == null)
        {
            offset = new Random().Next(tracks.Total ?? 0);
        }
        CPH.LogInfo("Resuming playback...");
        await Spotify.Player.ResumePlayback(new PlayerResumePlaybackRequest()
        {
            DeviceId = primary.Id,
            OffsetParam = new PlayerResumePlaybackRequest.Offset()
            {
                Position = offset
            },
            ContextUri = $"spotify:playlist:{Playlist}"
        });
        CPH.LogInfo("Getting the next song...");
        Paging<PlaylistTrack<IPlayableItem>> nextSong = await Spotify.Playlists.GetItems(Playlist, new PlaylistGetItemsRequest()
        {
            Offset = offset,
            Limit = 1,
        });
        CPH.LogInfo("Playlist started!");
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

    public bool VoteOnSong(string user, long vote)
    {

        FullTrack track = CurrentlyPlaying;
        SimpleArtist artist = CurrentArtist;
        if (track is null || artist is null)
        {
            return false;
        }

        CPH.LogDebug($"Voting {vote} on {track.Name}");
        if (scores[track.Id] is null)
        {
            scores[track.Id] = new SongEntry()
            {
                Name = track.Name,
                Artist = artist.Name,
                Upvotes = new HashSet<string>(),
                Downvotes = new HashSet<string>()
            };
        }
        SongEntry entry = (SongEntry)scores[track.Id];
        if (vote > 0)
        {
            entry.Upvotes.Add(user);
            entry.Downvotes.Remove(user);
        }
        else
        {
            entry.Upvotes.Remove(user);
            entry.Downvotes.Add(user);
        }

        entry.Score = entry.Upvotes.Count - entry.Downvotes.Count;
        scores[track.Id] = entry;

        switch (vote)
        {
            case 1:
                CPH.SendMessage($"Yo, {track.Name} by {entry.Artist} is a BANGER song! Let's gooooo (song score: {entry.Score})");
                break;
            case -1:
                CPH.SendMessage($"I agree, {track.Name} by {entry.Artist} SUCKS! (song score: {entry.Score})");
                break;
        }

        return true;
    }
}
