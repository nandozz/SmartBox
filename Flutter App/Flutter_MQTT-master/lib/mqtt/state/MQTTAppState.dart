import 'package:flutter/cupertino.dart';

enum MQTTAppConnectionState { connected, disconnected, connecting }

class MQTTAppState with ChangeNotifier {
  MQTTAppConnectionState _appConnectionState =
      MQTTAppConnectionState.disconnected;
  String _receivedText = '';
  String _receivedList = '';
  String _historyText = '';
  String _id = '';
  String _key = '';
  bool _isLock = true;

  void changeLock(bool isLock) {
    _isLock = !isLock;
    notifyListeners();
  }

  void setLogin(String id, String key) {
    _id = id;
    _key = key;
    notifyListeners();
  }

  void setReceivedText(String text) {
    _receivedText = text;

    if (text.contains('List')) {
      _receivedList = text;
    } else if (text.contains('listempty')) {
      _receivedList = 'Empty';
    }

    if (text.contains('History')) {
      _historyText = text;
    } else if (text.contains('historyempty')) {
      _historyText = 'Empty';
    }

    // _historyText = _historyText + '\n' + _receivedText;
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

  String get getReceivedList => _receivedList;
  String get getReceivedText => _receivedText;
  String get getHistoryText => _historyText;
  String get getId => _id;
  String get getKey => _key;
  // String get getClearHistoryText => _historyText;

  bool get getState => _isLock;

  MQTTAppConnectionState get getAppConnectionState => _appConnectionState;
}
