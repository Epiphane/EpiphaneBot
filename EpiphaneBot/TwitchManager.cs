using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TwitchLib.Api;
using TwitchLib.Client;
using TwitchLib.Client.Enums;
using TwitchLib.Client.Events;
using TwitchLib.Client.Extensions;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Models;
using TwitchLib.Api.Services;
using TwitchLib.Api.Services.Events;
using TwitchLib.Api.Services.Events.LiveStreamMonitor;
using TwitchLib.Api.Helix.Models.Users.GetUserFollows;

public class TwitchManager
{
    //private LiveStreamMonitorService Monitor;
    //private TwitchAPI API;

    public TwitchManager()
    {
        /*
        API = new TwitchAPI();

        API.Settings.ClientId = "gp762nuuoqcoxypju8c569th9wz7q5";
        API.Settings.AccessToken = "yufmcn3vvd754ukhr2y8rbngfz5emd";

        GetUsersFollowsResponse response = API.Helix.Users.GetUsersFollowsAsync().Result;
        Console.WriteLine($"Follows: {response.TotalFollows}");
        Monitor = new LiveStreamMonitorService(API, 60);

        List<string> lst = new List<string> { "ID1", "ID2" };
        Monitor.SetChannelsById(lst);

        Monitor.OnStreamOnline += Monitor_OnStreamOnline;
        Monitor.OnStreamOffline += Monitor_OnStreamOffline;
        Monitor.OnStreamUpdate += Monitor_OnStreamUpdate;

        Monitor.OnServiceStarted += Monitor_OnServiceStarted;
        Monitor.OnChannelsSet += Monitor_OnChannelsSet;

        Monitor.Start(); //Keep at the end!
        */
    }

    private void Client_OnLog(object sender, OnLogArgs e)
    {
        Console.WriteLine($"[Twitch] {e.DateTime.ToString()}: {e.BotUsername} - {e.Data}");
    }

    private void Client_OnConnected(object sender, OnConnectedArgs e)
    {
        Console.WriteLine($"[Twitch] Connected to {e.AutoJoinChannel}");
    }

    private void Client_OnJoinedChannel(object sender, OnJoinedChannelArgs e)
    {
        Console.WriteLine("[Twitch] Bot connected via TwitchLib!");
        //client.SendMessage(e.Channel, "Hey guys! I am a bot connected via TwitchLib!");
    }

    private void Client_OnMessageReceived(object sender, OnMessageReceivedArgs e)
    {
        /*
        if (e.ChatMessage.Message.Contains("badword"))
            client.TimeoutUser(e.ChatMessage.Channel, e.ChatMessage.Username, TimeSpan.FromMinutes(30), "Bad word! 30 minute timeout!");
        */
    }

    private void Client_OnWhisperReceived(object sender, OnWhisperReceivedArgs e)
    {
        /*
        if (e.WhisperMessage.Username == "my_friend")
            client.SendWhisper(e.WhisperMessage.Username, "Hey! Whispers are so cool!!");
    }

    private void Client_OnNewFollow(object sender, e)
    {
        Console.WriteLine($"[Twitch] New subscriber! {e.Subscriber.DisplayName}");
        */
    }

    private void Client_OnNewSubscriber(object sender, OnNewSubscriberArgs e)
    {
        Console.WriteLine($"[Twitch] New subscriber! {e.Subscriber.DisplayName}");
        /*
        if (e.Subscriber.SubscriptionPlan == SubscriptionPlan.Prime)
            client.SendMessage(e.Channel, $"Welcome {e.Subscriber.DisplayName} to the substers! You just earned 500 points! So kind of you to use your Twitch Prime on this channel!");
        else
            client.SendMessage(e.Channel, $"Welcome {e.Subscriber.DisplayName} to the substers! You just earned 500 points!");
        */
    }
}
