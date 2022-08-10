using System;
using System.Collections.Generic;
using System.Management;
using System.Diagnostics;
using Streamer.bot.Plugin.Interface;

public class ExternalManager
{
    public delegate void ProcessStateHandler(string processName, bool running);

    private readonly IInlineInvokeProxy CPH;
    private readonly Dictionary<string, List<ProcessStateHandler>> processListeners = new Dictionary<string, List<ProcessStateHandler>>();

    public ExternalManager(IInlineInvokeProxy CPH)
    {
        this.CPH = CPH;
    }

    public void OnProcessState(string processName, ProcessStateHandler handler)
    {
        CPH.LogDebug($"OnProcessState: {processName}");
        if (!processListeners.ContainsKey(processName))
        {
            CPH.LogDebug($"Addubg {processName} handler");
            processListeners[processName] = new List<ProcessStateHandler>();
            WatchForProcessStart(processName);
            WatchForProcessEnd(processName);
        }

        CPH.LogDebug($"Adding handler for {processName}");
        processListeners[processName].Add(handler);

        // Issue current state
        CPH.LogDebug($"Querying initial state of {processName}");
        if (Process.GetProcessesByName(processName.Substring(0, processName.LastIndexOf('.'))).Length > 0)
        {
            CPH.LogDebug($"{processName} is running!");
            handler(processName, true);
        }
        else
        {
            CPH.LogDebug($"{processName} is not running.");
            handler(processName, false);
        }
    }

    public void OffProcessState(string processName, ProcessStateHandler handler)
    {
        if (processListeners[processName] == null)
        {
            return;
        }

        processListeners[processName].Remove(handler);
    }

    private ManagementEventWatcher WatchForProcessStart(string processName)
    {
        CPH.LogDebug($"Watching for start events on {processName}");
        string queryString =
            "SELECT TargetInstance" +
            "  FROM __InstanceCreationEvent " +
            "WITHIN  1 " +
            " WHERE TargetInstance ISA 'Win32_Process' " +
            "   AND TargetInstance.Name = '" + processName + "'";

        // The dot in the scope means use the current machine
        string scope = @"\\.\root\CIMV2";

        // Create a watcher and listen for events
        ManagementEventWatcher watcher = new ManagementEventWatcher(scope, queryString);
        watcher.EventArrived += ProcessStarted;
        watcher.Start();
        return watcher;
    }

    private ManagementEventWatcher WatchForProcessEnd(string processName)
    {
        CPH.LogDebug($"Watching for end events on {processName}");
        string queryString =
            "SELECT TargetInstance" +
            "  FROM __InstanceDeletionEvent " +
            "WITHIN  1 " +
            " WHERE TargetInstance ISA 'Win32_Process' " +
            "   AND TargetInstance.Name = '" + processName + "'";

        // The dot in the scope means use the current machine
        string scope = @"\\.\root\CIMV2";

        // Create a watcher and listen for events
        ManagementEventWatcher watcher = new ManagementEventWatcher(scope, queryString);
        watcher.EventArrived += ProcessEnded;
        watcher.Start();
        return watcher;
    }

    private void ProcessEnded(object sender, EventArrivedEventArgs e)
    {
        ManagementBaseObject targetInstance = (ManagementBaseObject)e.NewEvent.Properties["TargetInstance"].Value;
        string processName = targetInstance.Properties["Name"].Value.ToString();
        CPH.LogInfo(String.Format("{0} process ended", processName));
        if (processListeners.ContainsKey(processName))
        {
            processListeners[processName].ForEach(handler => handler(processName, false));
        }
    }

    private void ProcessStarted(object sender, EventArrivedEventArgs e)
    {
        ManagementBaseObject targetInstance = (ManagementBaseObject)e.NewEvent.Properties["TargetInstance"].Value;
        string processName = targetInstance.Properties["Name"].Value.ToString();
        CPH.LogInfo(String.Format("{0} process started", processName));
        if (processListeners.ContainsKey(processName))
        {
            processListeners[processName].ForEach(handler => handler(processName, true));
        }
    }
}
