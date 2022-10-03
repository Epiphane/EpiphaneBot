using System;
using System.Collections.Generic;
using Streamer.bot.Common.Events;
using Streamer.bot.Plugin.Interface;

public class CPHProxy : IInlineInvokeProxy
{
    private IInlineInvokeProxy Proxy;
    public bool Live { get; set; } = false;

    public CPHProxy(IInlineInvokeProxy proxy)
    {
        Proxy = proxy;
    }

    public string TwitchClientId => Proxy.TwitchClientId;

    public string TwitchOAuthToken => Proxy.TwitchOAuthToken;

    public void AddToCredits(string section, string value, bool json = true)
    {
        Proxy.AddToCredits(section, value, json);
    }

    public bool AddUserToGroup(int userId, string groupName)
    {
        return Proxy.AddUserToGroup(userId, groupName);
    }

    public bool AddUserToGroup(string userName, string groupName)
    {
        return Proxy.AddUserToGroup(userName, groupName);
    }

    public int Between(int min, int max)
    {
        return Proxy.Between(min, max);
    }

    public int BroadcastUdp(int port, object data)
    {
        return Proxy.BroadcastUdp(port, data);
    }

    public void CommandAddToAllUserCooldowns(string id, int seconds)
    {
        Proxy.CommandAddToAllUserCooldowns(id, seconds);
    }

    public void CommandAddToGlobalCooldown(string id, int seconds)
    {
        Proxy.CommandAddToGlobalCooldown(id, seconds);
    }

    public void CommandAddToUserCooldown(string id, int userId, int seconds)
    {
        Proxy.CommandAddToUserCooldown(id, userId, seconds);
    }

    public void CommandResetAllUserCooldowns(string id)
    {
        Proxy.CommandResetAllUserCooldowns(id);
    }

    public void CommandResetGlobalCooldown(string id)
    {
        Proxy.CommandResetGlobalCooldown(id);
    }

    public void CommandResetUserCooldown(string id, int userId)
    {
        Proxy.CommandResetUserCooldown(id, userId);
    }

    public void CommandSetGlobalCooldownDuration(string id, int seconds)
    {
        Proxy.CommandSetGlobalCooldownDuration(id, seconds);
    }

    public void CommandSetUserCooldownDuration(string id, int seconds)
    {
        Proxy.CommandSetUserCooldownDuration(id, seconds);
    }

    public global::Twitch.Common.Models.Api.ClipData CreateClip()
    {
        return Proxy.CreateClip();
    }

    public global::Twitch.Common.Models.Api.StreamMarker CreateStreamMarker(string description)
    {
        return Proxy.CreateStreamMarker(description);
    }

    public void DisableAction(string actionName)
    {
        Proxy.DisableAction(actionName);
    }

    public void DisableCommand(string id)
    {
        Proxy.DisableCommand(id);
    }

    public void DisableReward(string rewardId)
    {
        Proxy.DisableReward(rewardId);
    }

    public void DisableTimer(string timerName)
    {
        Proxy.DisableTimer(timerName);
    }

    public void EnableAction(string actionName)
    {
        Proxy.EnableAction(actionName);
    }

    public void EnableCommand(string id)
    {
        Proxy.EnableCommand(id);
    }

    public void EnableReward(string rewardId)
    {
        Proxy.EnableReward(rewardId);
    }

    public void EnableTimer(string timerName)
    {
        Proxy.EnableTimer(timerName);
    }

    public string EscapeString(string text)
    {
        return Proxy.EscapeString(text);
    }

    public bool ExecuteMethod(string executeCode, string methodName)
    {
        return Proxy.ExecuteMethod(executeCode, methodName);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetAllClips()
    {
        return Proxy.GetAllClips();
    }

    public List<global::Twitch.Common.Models.Api.Cheermote> GetCheermotes()
    {
        return Proxy.GetCheermotes();
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClips(int count)
    {
        return Proxy.GetClips(count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId)
    {
        return Proxy.GetClipsForGame(gameId);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, int count)
    {
        return Proxy.GetClipsForGame(gameId, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, DateTime start, DateTime end)
    {
        return Proxy.GetClipsForGame(gameId, start, end);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, DateTime start, DateTime end, int count)
    {
        return Proxy.GetClipsForGame(gameId, start, end, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, TimeSpan duration)
    {
        return Proxy.GetClipsForGame(gameId, duration);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, TimeSpan duration, int count)
    {
        return Proxy.GetClipsForGame(gameId, duration, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId)
    {
        return Proxy.GetClipsForUser(userId);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, int count)
    {
        return Proxy.GetClipsForUser(userId, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, DateTime start, DateTime end)
    {
        return Proxy.GetClipsForUser(userId, start, end);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, DateTime start, DateTime end, int count)
    {
        return Proxy.GetClipsForUser(userId, start, end, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, TimeSpan duration)
    {
        return Proxy.GetClipsForUser(userId, duration);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, TimeSpan duration, int count)
    {
        return Proxy.GetClipsForUser(userId, duration, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username)
    {
        return Proxy.GetClipsForUser(username);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string userName, int count)
    {
        return Proxy.GetClipsForUser(userName, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, DateTime start, DateTime end)
    {
        return Proxy.GetClipsForUser(username, start, end);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, DateTime start, DateTime end, int count)
    {
        return Proxy.GetClipsForUser(username, start, end, count);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, TimeSpan duration)
    {
        return Proxy.GetClipsForUser(username, duration);
    }

    public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, TimeSpan duration, int count)
    {
        return Proxy.GetClipsForUser(username, duration, count);
    }

    public EventType GetEventType()
    {
        return Proxy.GetEventType();
    }

    public T GetGlobalVar<T>(string varName, bool persisted = true)
    {
        return Proxy.GetGlobalVar<T>(varName, persisted);
    }

    public EventSource GetSource()
    {
        return Proxy.GetSource();
    }

    public List<global::Twitch.Common.Models.Api.TeamInfo> GetTeamInfo(int userId)
    {
        return Proxy.GetTeamInfo(userId);
    }

    public List<global::Twitch.Common.Models.Api.TeamInfo> GetTeamInfo(string username)
    {
        return Proxy.GetTeamInfo(username);
    }

    public T GetTwitchUserVar<T>(string userName, string varName, bool persisted = true)
    {
        return Proxy.GetTwitchUserVar<T>(userName, varName, persisted);
    }

    public T GetUserVar<T>(string userName, string varName, bool persisted = true)
    {
        return Proxy.GetUserVar<T>(userName, varName, persisted);
    }

    public T GetYouTubeUserVar<T>(string userName, string varName, bool persisted = true)
    {
        return Proxy.GetYouTubeUserVar<T>(userName, varName, persisted);
    }

    public void KeyboardPress(string keyPress)
    {
        Proxy.KeyboardPress(keyPress);
    }

    public void LogDebug(string logLine)
    {
        Proxy.LogDebug(logLine);
    }

    public void LogInfo(string logLine)
    {
        Proxy.LogInfo(logLine);
    }

    public void LogWarn(string logLine)
    {
        Proxy.LogWarn(logLine);
    }

    public double NextDouble()
    {
        return Proxy.NextDouble();
    }

    public bool ObsConnect(int connection = 0)
    {
        return Proxy.ObsConnect(connection);
    }

    public long ObsConvertColorHex(string colorHex)
    {
        return Proxy.ObsConvertColorHex(colorHex);
    }

    public long ObsConvertRgb(int a, int r, int g, int b)
    {
        return Proxy.ObsConvertRgb(a, r, g, b);
    }

    public void ObsDisconnect(int connection = 0)
    {
        Proxy.ObsDisconnect(connection);
    }

    public int ObsGetConnectionByName(string name)
    {
        return Proxy.ObsGetConnectionByName(name);
    }

    public string ObsGetCurrentScene(int connection = 0)
    {
        return Proxy.ObsGetCurrentScene(connection);
    }

    public List<string> ObsGetGroupSources(string scene, string groupName, int connection = 0)
    {
        return Proxy.ObsGetGroupSources(scene, groupName, connection);
    }

    public string ObsGetSceneItemProperties(string scene, string source, int connection = 0)
    {
        return Proxy.ObsGetSceneItemProperties(scene, source, connection);
    }

    public void ObsHideFilter(string scene, string filterName, int connection = 0)
    {
        Proxy.ObsHideFilter(scene, filterName, connection);
    }

    public void ObsHideFilter(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.ObsHideFilter(scene, source, filterName, connection);
    }

    public void ObsHideGroupsSources(string scene, string groupName, int connection = 0)
    {
        Proxy.ObsHideGroupsSources(scene, groupName, connection);
    }

    public void ObsHideScenesFilters(string scene, int connection = 0)
    {
        Proxy.ObsHideScenesFilters(scene, connection);
    }

    public void ObsHideSceneSources(string scene, int connection = 0)
    {
        Proxy.ObsHideSceneSources(scene, connection);
    }

    public void ObsHideSource(string scene, string source, int connection = 0)
    {
        Proxy.ObsHideSource(scene, source, connection);
    }

    public void ObsHideSourcesFilters(string scene, string source, int connection = 0)
    {
        Proxy.ObsHideSourcesFilters(scene, source, connection);
    }

    public bool ObsIsConnected(int connection = 0)
    {
        return Proxy.ObsIsConnected(connection);
    }

    public bool ObsIsFilterEnabled(string scene, string filterName, int connection = 0)
    {
        return Proxy.ObsIsFilterEnabled(scene, filterName, connection);
    }

    public bool ObsIsFilterEnabled(string scene, string source, string filterName, int connection = 0)
    {
        return Proxy.ObsIsFilterEnabled(scene, source, filterName, connection);
    }

    public bool ObsIsRecording(int connection = 0)
    {
        return Proxy.ObsIsRecording(connection);
    }

    public bool ObsIsSourceVisible(string scene, string source, int connection = 0)
    {
        return Proxy.ObsIsSourceVisible(scene, source, connection);
    }

    public bool ObsIsStreaming(int connection = 0)
    {
        return Proxy.ObsIsStreaming(connection);
    }

    public void ObsMediaNext(string scene, string source, int connection = 0)
    {
        Proxy.ObsMediaNext(scene, source, connection);
    }

    public void ObsMediaPause(string scene, string source, int connection = 0)
    {
        Proxy.ObsMediaPause(scene, source, connection);
    }

    public void ObsMediaPlay(string scene, string source, int connection = 0)
    {
        Proxy.ObsMediaPlay(scene, source, connection);
    }

    public void ObsMediaPrevious(string scene, string source, int connection = 0)
    {
        Proxy.ObsMediaPrevious(scene, source, connection);
    }

    public void ObsMediaRestart(string scene, string source, int connection = 0)
    {
        Proxy.ObsMediaRestart(scene, source, connection);
    }

    public void ObsMediaStop(string scene, string source, int connection = 0)
    {
        Proxy.ObsMediaStop(scene, source, connection);
    }

    public void ObsPauseRecording(int connection = 0)
    {
        Proxy.ObsPauseRecording(connection);
    }

    public void ObsReplayBufferSave(int connection = 0)
    {
        Proxy.ObsReplayBufferSave(connection);
    }

    public void ObsReplayBufferStart(int connection = 0)
    {
        Proxy.ObsReplayBufferStart(connection);
    }

    public void ObsReplayBufferStop(int connection = 0)
    {
        Proxy.ObsReplayBufferStop(connection);
    }

    public void ObsResumeRecording(int connection = 0)
    {
        Proxy.ObsResumeRecording(connection);
    }

    public string ObsSendRaw(string requestType, string data, int connection = 0)
    {
        return Proxy.ObsSendRaw(requestType, data, connection);
    }

    public void ObsSetBrowserSource(string scene, string source, string url, int connection = 0)
    {
        Proxy.ObsSetBrowserSource(scene, source, url, connection);
    }

    public void ObsSetFilterState(string scene, string filterName, int state, int connection = 0)
    {
        Proxy.ObsSetFilterState(scene, filterName, state, connection);
    }

    public void ObsSetFilterState(string scene, string source, string filterName, int state, int connection = 0)
    {
        Proxy.ObsSetFilterState(scene, source, filterName, state, connection);
    }

    public void ObsSetGdiText(string scene, string source, string text, int connection = 0)
    {
        Proxy.ObsSetGdiText(scene, source, text, connection);
    }

    public void ObsSetImageSourceFile(string scene, string source, string file, int connection = 0)
    {
        Proxy.ObsSetImageSourceFile(scene, source, file, connection);
    }

    public void ObsSetMediaSourceFile(string scene, string source, string file, int connection = 0)
    {
        Proxy.ObsSetMediaSourceFile(scene, source, file, connection);
    }

    public void ObsSetMediaState(string scene, string source, int state, int connection = 0)
    {
        Proxy.ObsSetMediaState(scene, source, state, connection);
    }

    public void ObsSetRandomFilterState(string scene, int state, int connection = 0)
    {
        Proxy.ObsSetRandomFilterState(scene, state, connection);
    }

    public void ObsSetRandomFilterState(string scene, string source, int state, int connection = 0)
    {
        Proxy.ObsSetRandomFilterState(scene, source, state, connection);
    }

    public string ObsSetRandomGroupSourceVisible(string scene, string groupName, int connection = 0)
    {
        return Proxy.ObsSetRandomGroupSourceVisible(scene, groupName, connection);
    }

    public string ObsSetRandomSceneSourceVisible(string scene, int connection = 0)
    {
        return Proxy.ObsSetRandomSceneSourceVisible(scene, connection);
    }

    public void ObsSetReplayBufferState(int state, int connection = 0)
    {
        Proxy.ObsSetReplayBufferState(state, connection);
    }

    public void ObsSetScene(string sceneName, int connection = 0)
    {
        Proxy.ObsSetScene(sceneName, connection);
    }

    public void ObsSetSourceMuteState(string scene, string source, int state, int connection = 0)
    {
        Proxy.ObsSetSourceMuteState(scene, source, state, connection);
    }

    public void ObsSetSourceVisibility(string scene, string source, bool visible, int connection = 0)
    {
        Proxy.ObsSetSourceVisibility(scene, source, visible, connection);
    }

    public void ObsSetSourceVisibilityState(string scene, string source, int state, int connection = 0)
    {
        Proxy.ObsSetSourceVisibilityState(scene, source, state, connection);
    }

    public void ObsShowFilter(string scene, string filterName, int connection = 0)
    {
        Proxy.ObsShowFilter(scene, filterName, connection);
    }

    public void ObsShowFilter(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.ObsShowFilter(scene, source, filterName, connection);
    }

    public void ObsShowSource(string scene, string source, int connection = 0)
    {
        Proxy.ObsShowSource(scene, source, connection);
    }

    public void ObsSourceMute(string scene, string source, int connection = 0)
    {
        Proxy.ObsSourceMute(scene, source, connection);
    }

    public void ObsSourceMuteToggle(string scene, string source, int connection = 0)
    {
        Proxy.ObsSourceMuteToggle(scene, source, connection);
    }

    public void ObsSourceUnMute(string scene, string source, int connection = 0)
    {
        Proxy.ObsSourceUnMute(scene, source, connection);
    }

    public void ObsStartRecording(int connection = 0)
    {
        if (Live)
        {
            Proxy.ObsStartRecording(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Start Recording (OBS)");
        }
    }

    public void ObsStartStreaming(int connection = 0)
    {
        if (Live)
        {
            Proxy.ObsStartStreaming(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Start Streaming (OBS)");
        }
    }

    public void ObsStopRecording(int connection = 0)
    {
        if (Live)
        {
            Proxy.ObsStopRecording(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Stop Recording (OBS)");
        }
    }

    public void ObsStopStreaming(int connection = 0)
    {
        if (Live)
        {
            Proxy.ObsStopStreaming(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Stop Streaming (OBS)");
        }
    }

    public void ObsToggleFilter(string scene, string filterName, int connection = 0)
    {
        Proxy.ObsToggleFilter(scene, filterName, connection);
    }

    public void ObsToggleFilter(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.ObsToggleFilter(scene, source, filterName, connection);
    }

    public void PauseActionQueue(string name)
    {
        Proxy.PauseActionQueue(name);
    }

    public void PauseAllActionQueues()
    {
        Proxy.PauseAllActionQueues();
    }

    public void PauseReward(string rewardId)
    {
        Proxy.PauseReward(rewardId);
    }

    public void PlaySound(string fileName, float volume = 1, bool finishBeforeContinuing = false)
    {
        Proxy.PlaySound(fileName, volume, finishBeforeContinuing);
    }

    public void PlaySoundFromFolder(string path, float volume = 1, bool recursive = false, bool finishBeforeContinuing = false)
    {
        Proxy.PlaySoundFromFolder(path, volume, recursive, finishBeforeContinuing);
    }

    public bool RemoveUserFromGroup(int userId, string groupName)
    {
        return Proxy.RemoveUserFromGroup(userId, groupName);
    }

    public bool RemoveUserFromGroup(string userName, string groupName)
    {
        return Proxy.RemoveUserFromGroup(userName, groupName);
    }

    public void ResetCredits()
    {
        Proxy.ResetCredits();
    }

    public void ResetFirstWords()
    {
        Proxy.ResetFirstWords();
    }

    public void ResumeActionQueue(string name, bool clear = false)
    {
        Proxy.ResumeActionQueue(name, clear);
    }

    public void ResumeAllActionQueues(bool clear = false)
    {
        Proxy.ResumeAllActionQueues(clear);
    }

    public bool RunAction(string actionName, bool runImmediately = true)
    {
        return Proxy.RunAction(actionName, runImmediately);
    }

    public bool RunActionById(string actionId, bool runImmediately = true)
    {
        return Proxy.RunActionById(actionId, runImmediately);
    }

    public void SendAction(string action, bool bot = true)
    {
        Proxy.SendAction(action, bot);
    }

    public void SendMessage(string message, bool bot = true)
    {
        if (message == "")
        {
            return;
        }

        if (Live)
        {
            Proxy.SendMessage(message, bot);
        }
        else
        {
            Proxy.LogInfo($"[CPH] Chat messsage{(bot ? " by bot" : "")}: {message}");
        }
    }

    public void SendWhisper(string userName, string message)
    {
        Proxy.SendWhisper(userName, message);
    }

    public void SendYouTubeMessage(string message)
    {
        Proxy.SendYouTubeMessage(message);
    }

    public void SetArgument(string variableName, object value)
    {
        Proxy.SetArgument(variableName, value);
    }

    public GameInfo SetChannelGame(string game)
    {
        return Proxy.SetChannelGame(game);
    }

    public bool SetChannelGameById(string gameId)
    {
        return Proxy.SetChannelGameById(gameId);
    }

    public bool SetChannelTitle(string title)
    {
        return Proxy.SetChannelTitle(title);
    }

    public void SetGlobalVar(string varName, object value, bool persisted = true)
    {
        Proxy.SetGlobalVar(varName, value, persisted);
    }

    public void SetTwitchUserVar(string userName, string varName, object value, bool persisted = true)
    {
        Proxy.SetTwitchUserVar(userName, varName, value, persisted);
    }

    public void SetUserVar(string userName, string varName, object value, bool persisted = true)
    {
        Proxy.SetUserVar(userName, varName, value, persisted);
    }

    public void SetYouTubeUserVar(string userName, string varName, object value, bool persisted = true)
    {
        Proxy.SetYouTubeUserVar(userName, varName, value, persisted);
    }

    public bool SlobsConnect(int connection = 0)
    {
        return Proxy.SlobsConnect(connection);
    }

    public void SlobsDisconnect(int connection = 0)
    {
        Proxy.SlobsDisconnect(connection);
    }

    public string SlobsGetCurrentScene(int connection = 0)
    {
        return Proxy.SlobsGetCurrentScene(connection);
    }

    public List<string> SlobsGetGroupSources(string scene, string groupName, int connection = 0)
    {
        return Proxy.SlobsGetGroupSources(scene, groupName, connection);
    }

    public void SlobsHideFilter(string scene, string filterName, int connection = 0)
    {
        Proxy.SlobsHideFilter(scene, filterName, connection);
    }

    public void SlobsHideFilter(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.SlobsHideFilter(scene, source, filterName, connection);
    }

    public void SlobsHideGroupsSources(string scene, string groupName, int connection = 0)
    {
        Proxy.SlobsHideGroupsSources(scene, groupName, connection);
    }

    public void SlobsHideSource(string scene, string source, int connection = 0)
    {
        Proxy.SlobsHideSource(scene, source, connection);
    }

    public bool SlobsIsConnected(int connection = 0)
    {
        return Proxy.SlobsIsConnected(connection);
    }

    public bool SlobsIsFilterEnabled(string scene, string filterName, int connection = 0)
    {
        return Proxy.SlobsIsFilterEnabled(scene, filterName, connection);
    }

    public bool SlobsIsFilterEnabled(string scene, string source, string filterName, int connection = 0)
    {
        return Proxy.SlobsIsFilterEnabled(scene, source, filterName, connection);
    }

    public bool SlobsIsRecording(int connection = 0)
    {
        return Proxy.SlobsIsRecording(connection);
    }

    public bool SlobsIsSourceVisible(string scene, string source, int connection = 0)
    {
        return Proxy.SlobsIsSourceVisible(scene, source, connection);
    }

    public bool SlobsIsStreaming(int connection = 0)
    {
        return Proxy.SlobsIsStreaming(connection);
    }

    public void SlobsPauseRecording(int connection = 0)
    {
        Proxy.SlobsPauseRecording(connection);
    }

    public void SlobsResumeRecording(int connection = 0)
    {
        Proxy.SlobsResumeRecording(connection);
    }

    public void SlobsSetBrowserSource(string scene, string source, string url, int connection = 0)
    {
        Proxy.SlobsSetBrowserSource(scene, source, url, connection);
    }

    public void SlobsSetFilterState(string scene, string filterName, int state, int connection = 0)
    {
        Proxy.SlobsSetFilterState(scene, filterName, state, connection);
    }

    public void SlobsSetFilterState(string scene, string source, string filterName, int state, int connection = 0)
    {
        Proxy.SlobsSetFilterState(scene, source, filterName, state, connection);
    }

    public void SlobsSetGdiText(string scene, string source, string text, int connection = 0)
    {
        Proxy.SlobsSetGdiText(scene, source, text, connection);
    }

    public void SlobsSetRandomFilterState(string scene, int state, int connection = 0)
    {
        Proxy.SlobsSetRandomFilterState(scene, state, connection);
    }

    public void SlobsSetRandomFilterState(string scene, string source, int state, int connection = 0)
    {
        Proxy.SlobsSetRandomFilterState(scene, source, state, connection);
    }

    public string SlobsSetRandomGroupSourceVisible(string scene, string groupName, int connection = 0)
    {
        return Proxy.SlobsSetRandomGroupSourceVisible(scene, groupName, connection);
    }

    public void SlobsSetScene(string sceneName, int connection = 0)
    {
        Proxy.SlobsSetScene(sceneName, connection);
    }

    public void SlobsSetSourceMuteState(string scene, string source, int state, int connection = 0)
    {
        Proxy.SlobsSetSourceMuteState(scene, source, state, connection);
    }

    public void SlobsSetSourceVisibility(string scene, string source, bool visible, int connection = 0)
    {
        Proxy.SlobsSetSourceVisibility(scene, source, visible, connection);
    }

    public void SlobsSetSourceVisibilityState(string scene, string source, int state, int connection = 0)
    {
        Proxy.SlobsSetSourceVisibilityState(scene, source, state, connection);
    }

    public void SlobsShowFilter(string scene, string filterName, int connection = 0)
    {
        Proxy.SlobsShowFilter(scene, filterName, connection);
    }

    public void SlobsShowFilter(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.SlobsShowFilter(scene, source, filterName, connection);
    }

    public void SlobsShowSource(string scene, string source, int connection = 0)
    {
        Proxy.SlobsShowSource(scene, source, connection);
    }

    public void SlobsSourceMute(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.SlobsSourceMute(scene, source, filterName, connection);
    }

    public void SlobsSourceMuteToggle(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.SlobsSourceMuteToggle(scene, source, filterName, connection);
    }

    public void SlobsSourceUnMute(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.SlobsSourceUnMute(scene, source, filterName, connection);
    }

    public void SlobsStartRecording(int connection = 0)
    {
        if (Live)
        {
            Proxy.SlobsStartRecording(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Start Recording (SLOBS)");
        }
    }

    public void SlobsStartStreaming(int connection = 0)
    {
        if (Live)
        {
            Proxy.SlobsStartStreaming(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Start Streaming (SLOBS)");
        }
    }

    public void SlobsStopRecording(int connection = 0)
    {
        if (Live)
        {
            Proxy.SlobsStopRecording(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Stop Recording (SLOBS)");
        }
    }

    public void SlobsStopStreaming(int connection = 0)
    {
        if (Live)
        {
            Proxy.SlobsStopStreaming(connection);
        }
        else
        {
            Proxy.LogInfo("[CPH] Stop Streaming (SLOBS)");
        }
    }

    public void SlobsToggleFilter(string scene, string filterName, int connection = 0)
    {
        Proxy.SlobsToggleFilter(scene, filterName, connection);
    }

    public void SlobsToggleFilter(string scene, string source, string filterName, int connection = 0)
    {
        Proxy.SlobsToggleFilter(scene, source, filterName, connection);
    }

    public void TimeoutUser(string userName, int duration)
    {
        Proxy.TimeoutUser(userName, duration);
    }

    public int TtsSpeak(string voiceAlias, string message, bool badWordFilter = false)
    {
        return Proxy.TtsSpeak(voiceAlias, message, badWordFilter);
    }

    public void TwitchAnnounce(string message, bool bot = false, string color = null)
    {
        Proxy.TwitchAnnounce(message, bot, color);
    }

    public void TwitchEmoteOnly(bool enabled = true)
    {
        Proxy.TwitchEmoteOnly(enabled);
    }

    public void TwitchPollArchive(string pollId)
    {
        Proxy.TwitchPollArchive(pollId);
    }

    public bool TwitchPollCreate(string title, List<string> choices, int duration, int bitsPerVote = 0, int channelPointsPerVote = 0)
    {
        return Proxy.TwitchPollCreate(title, choices, duration, bitsPerVote);
    }

    public void TwitchPollTerminate(string pollId)
    {
        Proxy.TwitchPollTerminate(pollId);
    }

    public void TwitchPredictionCancel(string predictionId)
    {
        Proxy.TwitchPredictionCancel(predictionId);
    }

    public string TwitchPredictionCreate(string title, string firstOption, string secondOption, int duration)
    {
        return Proxy.TwitchPredictionCreate(title, firstOption, secondOption, duration);
    }

    public string TwitchPredictionCreate(string title, List<string> options, int duration)
    {
        return Proxy.TwitchPredictionCreate(title, options, duration);
    }

    public void TwitchPredictionLock(string predictionId)
    {
        Proxy.TwitchPredictionLock(predictionId);
    }

    public void TwitchPredictionResolve(string predictionId, string winningId)
    {
        Proxy.TwitchPredictionResolve(predictionId, winningId);
    }

    public bool TwitchRedemptionCancel(string rewardId, string redemptionId)
    {
        return Proxy.TwitchRedemptionCancel(rewardId, redemptionId);
    }

    public bool TwitchRedemptionFulfill(string rewardId, string redemptionId)
    {
        return Proxy.TwitchRedemptionFulfill(rewardId, redemptionId);
    }

    public void TwitchRunCommercial(int duration)
    {
        Proxy.TwitchRunCommercial(duration);
    }

    public void TwitchSlowMode(bool enabled = true, int duration = 0)
    {
        Proxy.TwitchSlowMode(enabled, duration);
    }

    public void TwitchSubscriberOnly(bool enabled = true)
    {
        Proxy.TwitchSubscriberOnly(enabled);
    }

    public void UnPauseReward(string rewardId)
    {
        Proxy.UnPauseReward(rewardId);
    }

    public void UnsetGlobalVar(string varName, bool persisted = true)
    {
        Proxy.UnsetGlobalVar(varName, persisted);
    }

    public void UnsetTwitchUser(string userName, bool persisted = true)
    {
        Proxy.UnsetTwitchUser(userName, persisted);
    }

    public void UnsetTwitchUserVar(string userName, string varName, bool persisted = true)
    {
        Proxy.UnsetTwitchUserVar(userName, varName, persisted);
    }

    public void UnsetUser(string userName, bool persisted = true)
    {
        Proxy.UnsetUser(userName, persisted);
    }

    public void UnsetUserVar(string userName, string varName, bool persisted = true)
    {
        Proxy.UnsetUserVar(userName, varName, persisted);
    }

    public void UnsetYouTubeUser(string userName, bool persisted = true)
    {
        Proxy.UnsetYouTubeUser(userName, persisted);
    }

    public void UnsetYouTubeUserVar(string userName, string varName, bool persisted = true)
    {
        Proxy.UnsetYouTubeUserVar(userName, varName, persisted);
    }

    public bool UpdateReward(string rewardId, string title = null, string prompt = null, int? cost = null)
    {
        return Proxy.UpdateReward(rewardId, title, prompt, cost);
    }

    public void UpdateRewardCooldown(string rewardId, int cooldown, bool additive = false)
    {
        Proxy.UpdateRewardCooldown(rewardId, cooldown, additive);
    }

    public void UpdateRewardCost(string rewardId, int cost, bool additive = false)
    {
        Proxy.UpdateRewardCost(rewardId, cost, additive);
    }

    public bool UpdateRewardPrompt(string rewardId, string prompt)
    {
        return Proxy.UpdateRewardPrompt(rewardId, prompt);
    }

    public bool UpdateRewardTitle(string rewardId, string title)
    {
        return Proxy.UpdateRewardTitle(rewardId, title);
    }

    public string UrlEncode(string text)
    {
        return Proxy.UrlEncode(text);
    }

    public bool UserInGroup(int userId, string groupName)
    {
        return Proxy.UserInGroup(userId, groupName);
    }

    public bool UserInGroup(string userName, string groupName)
    {
        return Proxy.UserInGroup(userName, groupName);
    }

    public void VoiceModBackgroundEffectOff()
    {
        Proxy.VoiceModBackgroundEffectOff();
    }

    public void VoiceModBackgroundEffectOn()
    {
        Proxy.VoiceModBackgroundEffectOn();
    }

    public void VoiceModCensorOff()
    {
        Proxy.VoiceModCensorOff();
    }

    public void VoiceModCensorOn()
    {
        Proxy.VoiceModCensorOn();
    }

    public bool VoiceModGetBackgroundEffectStatus()
    {
        return Proxy.VoiceModGetBackgroundEffectStatus();
    }

    public string VoiceModGetCurrentVoice()
    {
        return Proxy.VoiceModGetCurrentVoice();
    }

    public bool VoiceModGetHearMyselfStatus()
    {
        return Proxy.VoiceModGetHearMyselfStatus();
    }

    public bool VoiceModGetVoiceChangerStatus()
    {
        return Proxy.VoiceModGetVoiceChangerStatus();
    }

    public void VoiceModHearMyVoiceOff()
    {
        Proxy.VoiceModHearMyVoiceOff();
    }

    public void VoiceModHearMyVoiceOn()
    {
        Proxy.VoiceModHearMyVoiceOn();
    }

    public void VoiceModSelectVoice(string voiceId)
    {
        Proxy.VoiceModSelectVoice(voiceId);
    }

    public void VoiceModVoiceChangerOff()
    {
        Proxy.VoiceModVoiceChangerOff();
    }

    public void VoiceModVoiceChangerOn()
    {
        Proxy.VoiceModVoiceChangerOn();
    }

    public void Wait(int milliseconds)
    {
        Proxy.Wait(milliseconds);
    }

    public void WebsocketBroadcastJson(string data)
    {
        Proxy.WebsocketBroadcastJson(data);
    }

    public void WebsocketBroadcastString(string data)
    {
        Proxy.WebsocketBroadcastString(data);
    }

    public void WebsocketConnect(int connection = 0)
    {
        Proxy.WebsocketConnect(connection);
    }

    public void WebsocketCustomServerBroadcast(string data, string sessionId, int connection = 0)
    {
        Proxy.WebsocketCustomServerBroadcast(data, sessionId, connection);
    }

    public void WebsocketCustomServerCloseAllSessions(int connection = 0)
    {
        Proxy.WebsocketCustomServerCloseAllSessions(connection);
    }

    public void WebsocketCustomServerCloseSession(string sessionId, int connection = 0)
    {
        Proxy.WebsocketCustomServerCloseSession(sessionId, connection);
    }

    public int WebsocketCustomServerGetConnectionByName(string name)
    {
        return Proxy.WebsocketCustomServerGetConnectionByName(name);
    }

    public bool WebsocketCustomServerIsListening(int connection = 0)
    {
        return Proxy.WebsocketCustomServerIsListening(connection);
    }

    public void WebsocketCustomServerStart(int connection = 0)
    {
        Proxy.WebsocketCustomServerStart(connection);
    }

    public void WebsocketCustomServerStop(int connection = 0)
    {
        Proxy.WebsocketCustomServerStop(connection);
    }

    public void WebsocketDisconnect(int connection = 0)
    {
        Proxy.WebsocketDisconnect(connection);
    }

    public bool WebsocketIsConnected(int connection = 0)
    {
        return Proxy.WebsocketIsConnected(connection);
    }

    public void WebsocketSend(string data, int connection = 0)
    {
        Proxy.WebsocketSend(data, connection);
    }

    public void WebsocketSend(byte[] data, int connection = 0)
    {
        Proxy.WebsocketSend(data, connection);
    }

    public bool TwitchPollCreate(string title, List<string> choices, int duration, int channelPointsPerVote = 0)
    {
        return Proxy.TwitchPollCreate(title, choices, duration, channelPointsPerVote);
    }
}