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
            /*
            Settings settings = new Settings()
            {
                AccessToken = "",
                RefreshToken = "yello"
            };

            using FileStream createStream = File.Create("C:/Users/Thomas/AppData/Local/EpiphaneBot/Settings.json");
            await JsonSerializer.SerializeAsync(createStream, settings);
            */
        }


        private class User
        {
            public long Id { get; set; }
            public string Name { get; set; }
            public int Experience { get; set; }
            public int Wood { get; set; }
            public int Stone { get; set; }
            public int Iron { get; set; }
            public int Rubies { get; set; }
        }

        static void Main(string[] args)
        {
            Stack<string> stack = new Stack<string>();

            stack.Push("1");
            stack.Push("2");
            stack.Push("3");

            Console.WriteLine(stack.Peek());

            foreach(string s in stack)
            {
                Console.WriteLine(s);
            }
        }
    }
}
