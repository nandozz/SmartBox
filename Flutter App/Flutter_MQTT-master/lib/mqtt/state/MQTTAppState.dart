import 'package:flutter/cupertino.dart';

enum MQTTAppConnectionState { connected, disconnected, connecting }

class MQTTAppState with ChangeNotifier {
  MQTTAppConnectionState _appConnectionState =
      MQTTAppConnectionState.disconnected;
  String _receivedText = '';
  String _historyText = '';
  bool _isLock = true;

  void changeLock(bool isLock) {
    _isLock = !isLock;
    notifyListeners();
  }

  void setReceivedText(String text) {
    _receivedText = text;
    _historyText = _historyText + '\n' + _receivedText;
    notifyListeners();
  }

  void setAppConnectionState(MQTTAppConnectionState state) {
    _appConnectionState = state;
    notifyListeners();
  }

  void setClearHistoryText() {
    _historyText = '';
    notifyListeners();
  }

  String get getReceivedText => _receivedText;
  String get getHistoryText => _historyText;
  // String get getClearHistoryText => _historyText;

  bool get getState => _isLock;

  MQTTAppConnectionState get getAppConnectionState => _appConnectionState;
}
