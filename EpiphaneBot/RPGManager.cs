using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;

public class RPGManager
{
    public enum Resource
    {
        Wood,
        Stone,
        Iron,
    };

    private IInlineInvokeProxy CPH;

    public RPGManager(IInlineInvokeProxy CPH)
    {
        this.CPH = CPH;
    }

    public void AddResources(string userName, Resource resource, int amount)
    {
        string varName = $"RPGResource_{Enum.GetName(typeof(Resource), resource)}";
        int current = CPH.GetUserVar<int>(userName, varName);
        CPH.SetUserVar(userName, varName, current + amount);
    }
}
