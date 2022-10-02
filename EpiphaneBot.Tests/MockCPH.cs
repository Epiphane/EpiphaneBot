using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Streamer.bot.Common.Events;
using Streamer.bot.Plugin.Interface;

namespace EpiphaneBot.Tests
{
    public class MockCPH : IInlineInvokeProxy
    {
        public class StreamState
        {
            public bool Streaming = false;
            public bool Recording = false;
            public Queue<string> Messages = new Queue<string>();
            public List<string> ActionsRun = new List<string>();
            public Queue<int> Waits = new Queue<int>();
        }

        public readonly StreamState State = new StreamState();

        public class MockState
        {
            public Func<double> RandomDouble = () => 0.0;
        }

        public readonly MockState Mock = new MockState();

        public string TwitchClientId => "client-id";
        public string TwitchOAuthToken => "auth-token";

        private Queue<double> queuedDoubles = new Queue<double>();

        public MockCPH()
        {
            Mock.RandomDouble = () => queuedDoubles.Count > 0 ? queuedDoubles.Dequeue() : 0.0;
        }

        protected void AssertMessage(string message)
        {
            Assert.IsTrue(State.Messages.Count > 0);
            Assert.AreEqual(message, State.Messages.Dequeue());
        }

        protected void AssertWait(int milliseconds)
        {
            Assert.IsTrue(State.Waits.Count > 0);
            Assert.AreEqual(milliseconds, State.Waits.Dequeue());
        }

        protected void QueueDoubles(double[] values)
        {
            foreach (double value in values)
            {
                QueueDouble(value);
            }
        }

        protected void QueueDouble(double nextValue)
        {
            queuedDoubles.Enqueue(nextValue);
        }

        public void AddToCredits(string section, string value, bool json = true)
        {
            throw new NotImplementedException();
        }

        public bool AddUserToGroup(int userId, string groupName)
        {
            throw new NotImplementedException();
        }

        public bool AddUserToGroup(string userName, string groupName)
        {
            throw new NotImplementedException();
        }

        public int Between(int min, int max)
        {
            double val = Mock.RandomDouble();
            return (int)Math.Floor(val * (1 + max - min) + min);
        }

        public double NextDouble()
        {
            return Mock.RandomDouble();
        }

        public int BroadcastUdp(int port, object data)
        {
            throw new NotImplementedException();
        }

        public void CommandAddToAllUserCooldowns(string id, int seconds)
        {
            throw new NotImplementedException();
        }

        public void CommandAddToGlobalCooldown(string id, int seconds)
        {
            throw new NotImplementedException();
        }

        public void CommandAddToUserCooldown(string id, int userId, int seconds)
        {
            throw new NotImplementedException();
        }

        public void CommandResetAllUserCooldowns(string id)
        {
            throw new NotImplementedException();
        }

        public void CommandResetGlobalCooldown(string id)
        {
            throw new NotImplementedException();
        }

        public void CommandResetUserCooldown(string id, int userId)
        {
            throw new NotImplementedException();
        }

        public void CommandSetGlobalCooldownDuration(string id, int seconds)
        {
            throw new NotImplementedException();
        }

        public void CommandSetUserCooldownDuration(string id, int seconds)
        {
            throw new NotImplementedException();
        }

        public global::Twitch.Common.Models.Api.ClipData CreateClip()
        {
            throw new NotImplementedException();
        }

        public global::Twitch.Common.Models.Api.StreamMarker CreateStreamMarker(string description)
        {
            throw new NotImplementedException();
        }

        public void DisableAction(string actionName)
        {
            throw new NotImplementedException();
        }

        public void DisableCommand(string id)
        {
            throw new NotImplementedException();
        }

        public void DisableReward(string rewardId)
        {
            throw new NotImplementedException();
        }

        public void DisableTimer(string timerName)
        {
            throw new NotImplementedException();
        }

        public void EnableAction(string actionName)
        {
            throw new NotImplementedException();
        }

        public void EnableCommand(string id)
        {
            throw new NotImplementedException();
        }

        public void EnableReward(string rewardId)
        {
            throw new NotImplementedException();
        }

        public void EnableTimer(string timerName)
        {
            throw new NotImplementedException();
        }

        public string EscapeString(string text)
        {
            throw new NotImplementedException();
        }

        public bool ExecuteMethod(string executeCode, string methodName)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetAllClips()
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.Cheermote> GetCheermotes()
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClips(int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, DateTime start, DateTime end)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, DateTime start, DateTime end, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, TimeSpan duration)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForGame(int gameId, TimeSpan duration, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, DateTime start, DateTime end)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, DateTime start, DateTime end, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, TimeSpan duration)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(int userId, TimeSpan duration, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string userName, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, DateTime start, DateTime end)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, DateTime start, DateTime end, int count)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, TimeSpan duration)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.ClipData> GetClipsForUser(string username, TimeSpan duration, int count)
        {
            throw new NotImplementedException();
        }

        public EventType GetEventType()
        {
            throw new NotImplementedException();
        }

        public T GetGlobalVar<T>(string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public EventSource GetSource()
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.TeamInfo> GetTeamInfo(int userId)
        {
            throw new NotImplementedException();
        }

        public List<global::Twitch.Common.Models.Api.TeamInfo> GetTeamInfo(string username)
        {
            throw new NotImplementedException();
        }

        public T GetTwitchUserVar<T>(string userName, string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public T GetUserVar<T>(string userName, string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public T GetYouTubeUserVar<T>(string userName, string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void KeyboardPress(string keyPress)
        {
            throw new NotImplementedException();
        }

        public void LogDebug(string logLine)
        {
            Console.WriteLine($"[DBG] {logLine}");
        }

        public void LogInfo(string logLine)
        {
            Console.WriteLine($"[INF] {logLine}");
        }

        public void LogWarn(string logLine)
        {
            Console.WriteLine($"[ERR] {logLine}");
        }

        public bool ObsConnect(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public long ObsConvertColorHex(string colorHex)
        {
            throw new NotImplementedException();
        }

        public long ObsConvertRgb(int a, int r, int g, int b)
        {
            throw new NotImplementedException();
        }

        public void ObsDisconnect(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public int ObsGetConnectionByName(string name)
        {
            throw new NotImplementedException();
        }

        public string ObsGetCurrentScene(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public List<string> ObsGetGroupSources(string scene, string groupName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public string ObsGetSceneItemProperties(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideFilter(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideFilter(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideGroupsSources(string scene, string groupName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideScenesFilters(string scene, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideSceneSources(string scene, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideSource(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsHideSourcesFilters(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool ObsIsConnected(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool ObsIsFilterEnabled(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool ObsIsFilterEnabled(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool ObsIsRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool ObsIsSourceVisible(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool ObsIsStreaming(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsMediaNext(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsMediaPause(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsMediaPlay(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsMediaPrevious(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsMediaRestart(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsMediaStop(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsPauseRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsReplayBufferSave(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsReplayBufferStart(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsReplayBufferStop(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsResumeRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public string ObsSendRaw(string requestType, string data, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetBrowserSource(string scene, string source, string url, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetFilterState(string scene, string filterName, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetFilterState(string scene, string source, string filterName, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetGdiText(string scene, string source, string text, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetImageSourceFile(string scene, string source, string file, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetMediaSourceFile(string scene, string source, string file, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetMediaState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetRandomFilterState(string scene, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetRandomFilterState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public string ObsSetRandomGroupSourceVisible(string scene, string groupName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public string ObsSetRandomSceneSourceVisible(string scene, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetReplayBufferState(int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetScene(string sceneName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetSourceMuteState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetSourceVisibility(string scene, string source, bool visible, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSetSourceVisibilityState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsShowFilter(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsShowFilter(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsShowSource(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSourceMute(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSourceMuteToggle(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsSourceUnMute(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsStartRecording(int connection = 0)
        {
            State.Recording = true;
        }

        public void ObsStartStreaming(int connection = 0)
        {
            State.Streaming = true;
        }

        public void ObsStopRecording(int connection = 0)
        {
            State.Recording = false;
        }

        public void ObsStopStreaming(int connection = 0)
        {
            State.Streaming = false;
        }

        public void ObsToggleFilter(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void ObsToggleFilter(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void PauseActionQueue(string name)
        {
            throw new NotImplementedException();
        }

        public void PauseAllActionQueues()
        {
            throw new NotImplementedException();
        }

        public void PauseReward(string rewardId)
        {
            throw new NotImplementedException();
        }

        public void PlaySound(string fileName, float volume = 1, bool finishBeforeContinuing = false)
        {
            throw new NotImplementedException();
        }

        public void PlaySoundFromFolder(string path, float volume = 1, bool recursive = false, bool finishBeforeContinuing = false)
        {
            throw new NotImplementedException();
        }

        public bool RemoveUserFromGroup(int userId, string groupName)
        {
            throw new NotImplementedException();
        }

        public bool RemoveUserFromGroup(string userName, string groupName)
        {
            throw new NotImplementedException();
        }

        public void ResetCredits()
        {
            throw new NotImplementedException();
        }

        public void ResetFirstWords()
        {
            throw new NotImplementedException();
        }

        public void ResumeActionQueue(string name, bool clear = false)
        {
            throw new NotImplementedException();
        }

        public void ResumeAllActionQueues(bool clear = false)
        {
            throw new NotImplementedException();
        }

        public bool RunAction(string actionName, bool runImmediately = true)
        {
            State.ActionsRun.Add(actionName);
            return true;
        }

        public bool RunActionById(string actionId, bool runImmediately = true)
        {
            throw new NotImplementedException();
        }

        public void SendAction(string action, bool bot = true)
        {
            throw new NotImplementedException();
        }

        public void SendMessage(string message, bool bot = true)
        {
            State.Messages.Enqueue(message);
        }

        public void SendWhisper(string userName, string message)
        {
            throw new NotImplementedException();
        }

        public void SendYouTubeMessage(string message)
        {
            throw new NotImplementedException();
        }

        public void SetArgument(string variableName, object value)
        {
            throw new NotImplementedException();
        }

        public GameInfo SetChannelGame(string game)
        {
            throw new NotImplementedException();
        }

        public bool SetChannelGameById(string gameId)
        {
            throw new NotImplementedException();
        }

        public bool SetChannelTitle(string title)
        {
            throw new NotImplementedException();
        }

        public void SetGlobalVar(string varName, object value, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void SetTwitchUserVar(string userName, string varName, object value, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void SetUserVar(string userName, string varName, object value, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void SetYouTubeUserVar(string userName, string varName, object value, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public bool SlobsConnect(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsDisconnect(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public string SlobsGetCurrentScene(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public List<string> SlobsGetGroupSources(string scene, string groupName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsHideFilter(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsHideFilter(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsHideGroupsSources(string scene, string groupName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsHideSource(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool SlobsIsConnected(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool SlobsIsFilterEnabled(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool SlobsIsFilterEnabled(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool SlobsIsRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool SlobsIsSourceVisible(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool SlobsIsStreaming(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsPauseRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsResumeRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetBrowserSource(string scene, string source, string url, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetFilterState(string scene, string filterName, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetFilterState(string scene, string source, string filterName, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetGdiText(string scene, string source, string text, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetRandomFilterState(string scene, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetRandomFilterState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public string SlobsSetRandomGroupSourceVisible(string scene, string groupName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetScene(string sceneName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetSourceMuteState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetSourceVisibility(string scene, string source, bool visible, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSetSourceVisibilityState(string scene, string source, int state, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsShowFilter(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsShowFilter(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsShowSource(string scene, string source, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSourceMute(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSourceMuteToggle(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsSourceUnMute(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsStartRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsStartStreaming(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsStopRecording(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsStopStreaming(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsToggleFilter(string scene, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void SlobsToggleFilter(string scene, string source, string filterName, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void TimeoutUser(string userName, int duration)
        {
            throw new NotImplementedException();
        }

        public int TtsSpeak(string voiceAlias, string message, bool badWordFilter = false)
        {
            throw new NotImplementedException();
        }

        public void TwitchAnnounce(string message, bool bot = false, string color = null)
        {
            throw new NotImplementedException();
        }

        public void TwitchEmoteOnly(bool enabled = true)
        {
            throw new NotImplementedException();
        }

        public void TwitchPollArchive(string pollId)
        {
            throw new NotImplementedException();
        }

        public bool TwitchPollCreate(string title, List<string> choices, int duration, int bitsPerVote = 0, int channelPointsPerVote = 0)
        {
            throw new NotImplementedException();
        }

        public void TwitchPollTerminate(string pollId)
        {
            throw new NotImplementedException();
        }

        public void TwitchPredictionCancel(string predictionId)
        {
            throw new NotImplementedException();
        }

        public string TwitchPredictionCreate(string title, string firstOption, string secondOption, int duration)
        {
            throw new NotImplementedException();
        }

        public string TwitchPredictionCreate(string title, List<string> options, int duration)
        {
            throw new NotImplementedException();
        }

        public void TwitchPredictionLock(string predictionId)
        {
            throw new NotImplementedException();
        }

        public void TwitchPredictionResolve(string predictionId, string winningId)
        {
            throw new NotImplementedException();
        }

        public bool TwitchRedemptionCancel(string rewardId, string redemptionId)
        {
            throw new NotImplementedException();
        }

        public bool TwitchRedemptionFulfill(string rewardId, string redemptionId)
        {
            throw new NotImplementedException();
        }

        public void TwitchRunCommercial(int duration)
        {
            throw new NotImplementedException();
        }

        public void TwitchSlowMode(bool enabled = true, int duration = 0)
        {
            throw new NotImplementedException();
        }

        public void TwitchSubscriberOnly(bool enabled = true)
        {
            throw new NotImplementedException();
        }

        public void UnPauseReward(string rewardId)
        {
            throw new NotImplementedException();
        }

        public void UnsetGlobalVar(string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void UnsetTwitchUser(string userName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void UnsetTwitchUserVar(string userName, string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void UnsetUser(string userName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void UnsetUserVar(string userName, string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void UnsetYouTubeUser(string userName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public void UnsetYouTubeUserVar(string userName, string varName, bool persisted = true)
        {
            throw new NotImplementedException();
        }

        public bool UpdateReward(string rewardId, string title = null, string prompt = null, int? cost = null)
        {
            throw new NotImplementedException();
        }

        public void UpdateRewardCooldown(string rewardId, int cooldown, bool additive = false)
        {
            throw new NotImplementedException();
        }

        public void UpdateRewardCost(string rewardId, int cost, bool additive = false)
        {
            throw new NotImplementedException();
        }

        public bool UpdateRewardPrompt(string rewardId, string prompt)
        {
            throw new NotImplementedException();
        }

        public bool UpdateRewardTitle(string rewardId, string title)
        {
            throw new NotImplementedException();
        }

        public string UrlEncode(string text)
        {
            throw new NotImplementedException();
        }

        public bool UserInGroup(int userId, string groupName)
        {
            throw new NotImplementedException();
        }

        public bool UserInGroup(string userName, string groupName)
        {
            throw new NotImplementedException();
        }

        public void VoiceModBackgroundEffectOff()
        {
            throw new NotImplementedException();
        }

        public void VoiceModBackgroundEffectOn()
        {
            throw new NotImplementedException();
        }

        public void VoiceModCensorOff()
        {
            throw new NotImplementedException();
        }

        public void VoiceModCensorOn()
        {
            throw new NotImplementedException();
        }

        public bool VoiceModGetBackgroundEffectStatus()
        {
            throw new NotImplementedException();
        }

        public string VoiceModGetCurrentVoice()
        {
            throw new NotImplementedException();
        }

        public bool VoiceModGetHearMyselfStatus()
        {
            throw new NotImplementedException();
        }

        public bool VoiceModGetVoiceChangerStatus()
        {
            throw new NotImplementedException();
        }

        public void VoiceModHearMyVoiceOff()
        {
            throw new NotImplementedException();
        }

        public void VoiceModHearMyVoiceOn()
        {
            throw new NotImplementedException();
        }

        public void VoiceModSelectVoice(string voiceId)
        {
            throw new NotImplementedException();
        }

        public void VoiceModVoiceChangerOff()
        {
            throw new NotImplementedException();
        }

        public void VoiceModVoiceChangerOn()
        {
            throw new NotImplementedException();
        }

        public void Wait(int milliseconds)
        {
            State.Waits.Enqueue(milliseconds);
        }

        public void WebsocketBroadcastJson(string data)
        {
            throw new NotImplementedException();
        }

        public void WebsocketBroadcastString(string data)
        {
            throw new NotImplementedException();
        }

        public void WebsocketConnect(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketCustomServerBroadcast(string data, string sessionId, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketCustomServerCloseAllSessions(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketCustomServerCloseSession(string sessionId, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public int WebsocketCustomServerGetConnectionByName(string name)
        {
            throw new NotImplementedException();
        }

        public bool WebsocketCustomServerIsListening(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketCustomServerStart(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketCustomServerStop(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketDisconnect(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool WebsocketIsConnected(int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketSend(string data, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public void WebsocketSend(byte[] data, int connection = 0)
        {
            throw new NotImplementedException();
        }

        public bool TwitchPollCreate(string title, List<string> choices, int duration, int channelPointsPerVote = 0)
        {
            throw new NotImplementedException();
        }

        public void LumiaSendCommand(string command)
        {
            throw new NotImplementedException();
        }
    }
}
