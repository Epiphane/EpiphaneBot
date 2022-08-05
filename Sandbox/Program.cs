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

namespace Sandbox
{
    internal class Program
    {
        [Serializable]
        public struct Settings
        {
            public string AccessToken {  get; set; }
            public string RefreshToken { get; set; }
        }

        public async void GetCallback(string code)
        {
            var response = await new OAuthClient().RequestToken(
              new AuthorizationCodeTokenRequest("ClientId", "ClientSecret", code, new Uri("http://localhost:5000"))
            );

            var spotify = new SpotifyClient(response.AccessToken);
            // Also important for later: response.RefreshToken
        }

        public static async Task DoMain()
        {
            Settings settings = new Settings()
            {
                AccessToken = "",
                RefreshToken = "yello"
            };

            using FileStream createStream = File.Create("C:/Users/Thomas/AppData/Local/EpiphaneBot/Settings.json");
            await JsonSerializer.SerializeAsync(createStream, settings);
        }

        static void Main(string[] args)
        {
            DoMain().GetAwaiter().GetResult();

        /*
        TwitchAPI API = new TwitchAPI();
        API.Settings.ClientId = "sdgegftxqbmgbvvlcu2ojmz8mz8ub2";
        API.Settings.AccessToken = "1wrov4k6pkih4yyr75yv2zn89dnrdz";

        API.Settings.AccessToken = "lnn6k43ybj0h91l2dozpxo7gftc1pi";
        API.Settings.ClientId = "gp762nuuoqcoxypju8c569th9wz7q5";
        //5dbkue4f6anbccpbchq3p8inyqvv4ttf6tx4bg5xn3hm13m7v6
        //API.Settings

        // Client ID: sdgegftxqbmgbvvlcu2ojmz8mz8ub2
        // Client Secret: jd2tgxry63rkj18m3dj8byojkfz918

        //API.Auth.ValidateAccessTokenAsync
        //var result = API.Auth.RefreshAuthTokenAsync("ynfdpoqjd5tpexwyvzozvedf3dt29qvgm4oebszbccgrd7fniv", "jd2tgxry63rkj18m3dj8byojkfz918").Result;

        Task<GetUsersFollowsResponse> res = API.Helix.Users.GetUsersFollowsAsync();
        res.Wait();
        Console.WriteLine($"Follows: {res.Result.TotalFollows}");
        */

            // http://localhost:5000/
            // ?code=AQDO4gtohGFxSUJ54GhxTBP-ptx7iBFKlTzFuHkUEGtrtiJOwDrI7msVECj2tkWlv9XGZnWu01gJMV2bJANyXNP9jV7DOow0iYbFIldWIkYpBikSg88Z0zNyVOpkSkaB_Jj_iNdx9ad9aqiaDt6rNp5G4oiXFaxOcG7dl49LX9zEkhq012RjMwjOFmjB-S0rm7g0awFo8XT26DYFplxvIydHwbeIf33jCYYYoQyPU89ZmsaA6cMTu9A0dHxJJWr6RKkYQjpD5u6Q_BI3SkbLg4VXnmvpAEM9FL_KuRGAeha2UuQq2opPAcedXoq7eyzoV24gN_utfRzGAmiqNNIHDpVgyylOVkP6GBarRMmAqsTvgptCurN2Hn8F-M1TcRFNFFY6EvtxdJHOEFr8A9dMQQYCVtsw-uE7yLdYFw62_DwPcW5fkpo61NEGQW0leZ0hjoQZ1NwO6I0Cboa4z4v7vVtvi__YKOgp-nM1Q6_2002S5elt1EhWfhssG5jBeHMGjTbseTC2irRQBiEy3SbDgDEGZRdmqhdbCpeGYRY0Uj9dtiE-wg4a8nDyq1EOw9Um8Ml1fCRMlObKhHZABpyFBGVpnfsmJ2lUO69vOYcwfjkSUnsyXs1om85FgNcJGWbuvyHs7OI0vcze7S88nhuGlwGKxBlYhZGIH5XEkhvE7q70OLU4aW5KLBkXDqLw3EJ8I951HV3BoTCiNI0

            /*
            Uri redirect = new Uri("epiphane-bot://callback");
            redirect = new Uri("http://localhost:5000/");

            var loginRequest = new LoginRequest(
              redirect,//new Uri("http://localhost:5000"),
              "2b77d444e13e48cfb2effe8143831ae6",
              LoginRequest.ResponseType.Code
            )
            {
                Scope = new[] {
                    Scopes.UgcImageUpload,
                    Scopes.UserReadPlaybackState,
                    Scopes.UserModifyPlaybackState,
                    Scopes.UserReadCurrentlyPlaying,
                    Scopes.Streaming,
                    Scopes.AppRemoteControl,
                    Scopes.UserReadEmail,
                    Scopes.UserReadPrivate,
                    //Scopes.PlaylistReadCollaborative,
                    //Scopes.PlaylistModifyPublic,
                    //Scopes.PlaylistReadPrivate,
                    //Scopes.PlaylistModifyPrivate,
                    Scopes.UserLibraryModify,
                    Scopes.UserLibraryRead,
                    Scopes.UserTopRead,
                    Scopes.UserReadPlaybackPosition,
                    Scopes.UserReadRecentlyPlayed,
                    Scopes.UserFollowRead,
                    Scopes.UserFollowModify
                }
            };
            var uri = loginRequest.ToUri();

            Console.WriteLine(uri.ToString());

            string code = Console.ReadLine();

            OAuthClient client = new OAuthClient();
            //var code = "AQDO4gtohGFxSUJ54GhxTBP-ptx7iBFKlTzFuHkUEGtrtiJOwDrI7msVECj2tkWlv9XGZnWu01gJMV2bJANyXNP9jV7DOow0iYbFIldWIkYpBikSg88Z0zNyVOpkSkaB_Jj_iNdx9ad9aqiaDt6rNp5G4oiXFaxOcG7dl49LX9zEkhq012RjMwjOFmjB-S0rm7g0awFo8XT26DYFplxvIydHwbeIf33jCYYYoQyPU89ZmsaA6cMTu9A0dHxJJWr6RKkYQjpD5u6Q_BI3SkbLg4VXnmvpAEM9FL_KuRGAeha2UuQq2opPAcedXoq7eyzoV24gN_utfRzGAmiqNNIHDpVgyylOVkP6GBarRMmAqsTvgptCurN2Hn8F-M1TcRFNFFY6EvtxdJHOEFr8A9dMQQYCVtsw-uE7yLdYFw62_DwPcW5fkpo61NEGQW0leZ0hjoQZ1NwO6I0Cboa4z4v7vVtvi__YKOgp-nM1Q6_2002S5elt1EhWfhssG5jBeHMGjTbseTC2irRQBiEy3SbDgDEGZRdmqhdbCpeGYRY0Uj9dtiE-wg4a8nDyq1EOw9Um8Ml1fCRMlObKhHZABpyFBGVpnfsmJ2lUO69vOYcwfjkSUnsyXs1om85FgNcJGWbuvyHs7OI0vcze7S88nhuGlwGKxBlYhZGIH5XEkhvE7q70OLU4aW5KLBkXDqLw3EJ8I951HV3BoTCiNI0";
            var response = client.RequestToken(
                new AuthorizationCodeTokenRequest("2b77d444e13e48cfb2effe8143831ae6", "f92bd400e91745f6a27ead8d4df3376e", code, redirect)
            ).Result;
            // broken

            Console.WriteLine(response.AccessToken);
            Console.WriteLine(response.RefreshToken);
            */

            /* 
            {
                "access_token": "BQBJ7E8vd5a7533DXGCtnGqwD5PltozH0R2wTVGEhItVz2wi5tG2bPxzFUS6nBHHJX87CJrvkh9KOauv1oHOZB-gYSOrY4SWFXr0cqahdBnRSY83PQV_E7VLlqqf8UEcIINrKGBOapAHQNWPtnnYQ9Zky93maR9e-uwN9wwFC1EcS88XYAZsYF_lTpSbIFq_2Fqzh3LSRHa66XAV9qWv5Gh1gPe_wzJax0tC_KS4XOzbiolsS3kR6LzW",
                "token_type": "Bearer",
                "expires_in": 3600,
                "refresh_token": "AQBjXx1E0vPpc_VhBoyRRDXoUlMKLr2hQgoDMIcdIJnRWyH63uH9YEZ5F6poPm6ccnYQyjZOzNzazCT8FhzJZXvFYflD578r-xbE4YZV9oNmcYSt093oGPh7kD8UrA4HLRo",
                "scope": "ugc-image-upload user-follow-read user-read-email user-read-private streaming app-remote-control user-modify-playback-state user-follow-modify user-library-read user-library-modify user-read-playback-state user-read-currently-playing user-read-recently-played user-read-playback-position user-top-read"
            }
            */

            /*
            string AccessToken = "BQBJ7E8vd5a7533DXGCtnGqwD5PltozH0R2wTVGEhItVz2wi5tG2bPxzFUS6nBHHJX87CJrvkh9KOauv1oHOZB-gYSOrY4SWFXr0cqahdBnRSY83PQV_E7VLlqqf8UEcIINrKGBOapAHQNWPtnnYQ9Zky93maR9e-uwN9wwFC1EcS88XYAZsYF_lTpSbIFq_2Fqzh3LSRHa66XAV9qWv5Gh1gPe_wzJax0tC_KS4XOzbiolsS3kR6LzW";
            string RefreshToken = "AQBjXx1E0vPpc_VhBoyRRDXoUlMKLr2hQgoDMIcdIJnRWyH63uH9YEZ5F6poPm6ccnYQyjZOzNzazCT8FhzJZXvFYflD578r-xbE4YZV9oNmcYSt093oGPh7kD8UrA4HLRo";
            OAuthClient client = new OAuthClient();
            var newResponse = client.RequestToken(
                new AuthorizationCodeRefreshRequest("2b77d444e13e48cfb2effe8143831ae6", "f92bd400e91745f6a27ead8d4df3376e", RefreshToken)
            ).Result;

            Console.WriteLine(newResponse.AccessToken);
            Console.WriteLine(newResponse.RefreshToken);
            */
    }
    }
}
