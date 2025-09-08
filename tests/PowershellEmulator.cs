using System.Diagnostics.CodeAnalysis;
using System.Management.Automation;
using System.Management.Automation.Host;
using System.Runtime.Versioning;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    // Adapted from https://pawelszczygielski.pl/2021/05/06/testing-powershell-cmdlets-written-in-c/.
    [ExcludeFromCodeCoverageAttribute]
    internal class PowershellEmulator : ICommandRuntime {
        public List<object> OutputObjects { get; } = new List<object>();
        public PSHost? Host {
            get;
        }
        public PSTransactionContext? CurrentPSTransaction {
            get;
        }

        public bool ShouldContinue(string? query, string? caption, ref bool yesToAll,
            ref bool noToAll) {
            return true;
        }

        public bool ShouldContinue(string? query, string? caption) {
            return true;
        }

        public bool TransactionAvailable() {
            return true;
        }

        public bool ShouldProcess(string? verboseDescription, string? verboseWarning,
            string? caption, out ShouldProcessReason shouldProcessReason) {
            shouldProcessReason = ShouldProcessReason.None;
            return true;
        }

        public bool ShouldProcess(string? verboseDescription, string? verboseWarning,
            string? caption) {
            return true;
        }

        public bool ShouldProcess(string? target, string? action) {
            return true;
        }

        public bool ShouldProcess(string? target) {
            return true;
        }

        [DoesNotReturn]
        public void ThrowTerminatingError(ErrorRecord errorRecord) {
            throw new InvalidOperationException("Error in pipeline", errorRecord.Exception);
        }

        public void WriteCommandDetail(string text) {
            WriteObject(text);
        }

        public void WriteDebug(string text) {
            WriteObject(text);
        }

        public void WriteError(ErrorRecord errorRecord) {
            throw new InvalidOperationException("Error in pipeline", errorRecord.Exception);
        }

        public void WriteObject(object? sendToPipeline, bool enumerateCollection) {
            if (sendToPipeline != null) {
                WriteObject(sendToPipeline);
            }
        }

        public void WriteObject(object? sendToPipeline) {
            if (sendToPipeline != null) {
                OutputObjects.Add(sendToPipeline);
            }
        }

        public void WriteProgress(long sourceId, ProgressRecord progressRecord) {
            WriteObject(progressRecord);
        }

        public void WriteProgress(ProgressRecord progressRecord) {
            WriteObject(progressRecord);
        }

        public void WriteVerbose(string text) {
            WriteObject(text);
        }

        public void WriteWarning(string text) {
            WriteObject(text);
        }
    }
}
