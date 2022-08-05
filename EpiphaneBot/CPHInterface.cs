using System;
using System.Collections.Generic;

public class GameInfo
{
    public int Id { get; set; }
    public string Name { get; set; }
    public string BoxArtUrl { get; set; }
}

public partial class CPHInline
{
#pragma warning disable 0649
    private readonly Streamer.bot.Plugin.Interface.IInlineInvokeProxy CPH;
    private readonly Dictionary<string, Object> args;
#pragma warning restore 0649
}