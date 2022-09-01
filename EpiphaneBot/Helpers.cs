using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Streamer.bot.Plugin.Interface;

public static class Helpers
{
    public static void Shuffle<T>(IInlineInvokeProxy CPH, List<T> array)
    {
        int n = array.Count;
        while (n > 1)
        {
            int k = CPH.Between(0, --n);
            (array[k], array[n]) = (array[n], array[k]);
        }
    }

    public static void Shuffle<T>(IInlineInvokeProxy CPH, T[] array)
    {
        int n = array.Length;
        while (n > 1)
        {
            int k = CPH.Between(0, --n);
            (array[k], array[n]) = (array[n], array[k]);
        }
    }

    public static T Random<T>(IInlineInvokeProxy CPH, List<T> array)
    {
        return array[CPH.Between(0, array.Count - 1)];
    }

    public static T Random<T>(IInlineInvokeProxy CPH, T[] array)
    {
        return array[CPH.Between(0, array.Length - 1)];
    }
}
