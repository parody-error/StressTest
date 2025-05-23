﻿using System.Web;
using Prism.Commands;

namespace StressTest.ViewModels
{
    internal class ConnectionViewModel : ViewModelBase
    {
        public delegate void OnSelectAction( string sourceDSN, string username, string password, string database, string schema );
        public event OnSelectAction OnSelectEvent;

        public delegate void OnCancelAction();
        public event OnCancelAction OnCancelEvent;

        public DelegateCommand SelectCommand { get; }
        public DelegateCommand CancelCommand { get; }

        private string _sourceDSN = string.Empty;
        public string SourceDSN
        {
            get => _sourceDSN;
            set
            {
                _sourceDSN = value;
                UpdateCommandState();
            }
        }

        private string _database = string.Empty;
        public string Database
        {
            get => _database;
            set
            {
                _database = value;
                UpdateCommandState();
            }
        }

        private string _schema = string.Empty;
        public string Schema
        {
            get => _schema;
            set
            {
                _schema = value;
                UpdateCommandState();
            }
        }

        public string Username { get; set; } = string.Empty;
        public string Password { get; set; } = string.Empty;

        public ConnectionViewModel()
        {
            SelectCommand = new DelegateCommand(
                ExecuteSelectCommand,
                () => {
                    return
                    !string.IsNullOrEmpty(_sourceDSN) &&
                    !string.IsNullOrEmpty(_database) &&
                    !string.IsNullOrEmpty(_schema);
                }
            );

            CancelCommand = new DelegateCommand(
                ExecuteCancelCommand,
                () => { return true; }
            );
        }

        private void ExecuteSelectCommand()
        {
            OnSelectEvent?.Invoke( SourceDSN, Username, Password, Database, Schema );
        }

        private void ExecuteCancelCommand()
        {
            OnCancelEvent?.Invoke();
        }

        private void UpdateCommandState()
        {
            SelectCommand.RaiseCanExecuteChanged();
        }
    }
}
