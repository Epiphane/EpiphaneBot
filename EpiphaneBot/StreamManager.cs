using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;

 public class StreamManager
{
    private IInlineInvokeProxy CPH;

    public StreamManager(IInlineInvokeProxy CPH)
    {
        this.CPH = CPH;
    }

    private int firstsAvailable = 3;

    private List<string> claimants
    {
        set;
        get;
    } = new List<string>();

    public void SetFirstsAvailable(int firstsAvailable)
    {
        this.firstsAvailable = firstsAvailable;
    }

    public void ResetState()
    {
        claimants = new List<string>();
    }

    public bool ClaimFirst(string name, out int place)
    {
        if (claimants.Contains(name))
        {
            place = claimants.IndexOf(name);
            return false;
        }

        if (claimants.Count < firstsAvailable)
        {
            place = claimants.Count;
            claimants.Add(name);
            return true;
        }

        place = -1;
        return false;
    }
    
    public bool HasClaimed(string name)
    {
        return claimants.Contains(name);
    }

    public string Claimants
    {
        get {
            string claimStr = "";
            for (int i = 0; i < claimants.Count; i++)
            {
                claimStr += claimants[i];
                if (i < claimants.Count - 2)
                {
                    claimStr += ", ";
                }
                else if (i == claimants.Count - 2)
                {
                    claimStr += ", and ";
                }
            }
            return claimStr;
        }
    }
}
