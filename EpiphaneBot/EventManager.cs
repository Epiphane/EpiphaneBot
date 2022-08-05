using Streamer.bot.Plugin.Interface;

public class EventManager
{
    private IInlineInvokeProxy CPH;

    public EventManager(IInlineInvokeProxy CPH)
    {
        this.CPH = CPH;
    }

    public void OnFollow(string user)
    {
        CPH.LogInfo($"User length: {user.Length}");
        CPH.ObsSetGdiText("[E] Latest Follower Name", "Latest Follower Name", user + "    ");
        if (user.Length > 13)
        {
            CPH.ObsShowFilter("[E] Latest Follower Name", "Latest Follower Name", "Marquee");
            CPH.ObsShowFilter("[E] Latest Follower Name", "Latest Follower Name", "End Marquee");
        }
        else
        {
            CPH.ObsHideFilter("[E] Latest Follower Name", "Latest Follower Name", "Marquee");
        }
    }

    public void OnSubscribe(string user)
    {
        CPH.LogInfo($"User length: {user.Length}");
        CPH.ObsSetGdiText("[E] Latest Subscriber Name", "Latest Subscriber Name", user + "    ");
        if (user.Length > 13)
        {
            CPH.ObsShowFilter("[E] Latest Subscriber Name", "Latest Subscriber Name", "Marquee");
            CPH.ObsShowFilter("[E] Latest Subscriber Name", "Latest Subscriber Name", "End Marquee");
        }
        else
        {
            CPH.ObsHideFilter("[E] Latest Subscriber Name", "Latest Subscriber Name", "Marquee");
        }
    }
};
 