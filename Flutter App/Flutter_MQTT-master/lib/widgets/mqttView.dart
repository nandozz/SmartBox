import 'dart:io' show Platform;
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:flutter_mqtt_app/mqtt/state/MQTTAppState.dart';
import 'package:flutter_mqtt_app/mqtt/MQTTManager.dart';

class MQTTView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return _MQTTViewState();
  }
}

class _MQTTViewState extends State<MQTTView> {
  final TextEditingController _hostTextController = TextEditingController();
  final TextEditingController _messageTextController = TextEditingController();
  final TextEditingController _bokuIDTextController = TextEditingController();
  final TextEditingController _bokuPassTextController = TextEditingController();

  late MQTTAppState currentAppState;
  late MQTTManager manager;
  bool isLock = true;

  @override
  void initState() {
    super.initState();

    /*
    _hostTextController.addListener(_printLatestValue);
    _messageTextController.addListener(_printLatestValue);
    _bokuIDTextController.addListener(_printLatestValue);

     */
  }

  @override
  void dispose() {
    _hostTextController.dispose();
    _messageTextController.dispose();
    _bokuIDTextController.dispose();
    _bokuPassTextController.dispose();
    super.dispose();
  }

  /*
  _printLatestValue() {
    print("Second text field: ${_hostTextController.text}");
    print("Second text field: ${_messageTextController.text}");
    print("Second text field: ${_bokuIDTextController.text}");
  }

   */

  @override
  Widget build(BuildContext context) {
    final MQTTAppState appState = Provider.of<MQTTAppState>(context);
    // Keep a reference to the app state.
    currentAppState = appState;
    final Scaffold scaffold = Scaffold(body: SafeArea(child: _buildColumn()));
    return scaffold;
  }

  // Widget _buildAppBar(BuildContext context) {
  //   return AppBar(
  //     title: const Text('MQTT'),
  //     backgroundColor: Colors.greenAccent,
  //   );
  // }

  Widget _buildColumn() {
    return Column(
      children: <Widget>[
        _buildConnectionStateText(
            _prepareStateMessageFrom(currentAppState.getAppConnectionState)),
        _buildEditableColumn(),
        _buildScrollableTextWith(currentAppState.getHistoryText)
      ],
    );
  }

  Widget _buildEditableColumn() {
    return SingleChildScrollView(
      child: Padding(
        padding: const EdgeInsets.all(20.0),
        child: Column(
          children: <Widget>[
            // _buildTextFieldWith(_hostTextController, 'Enter broker address',
            //     currentAppState.getAppConnectionState),
            // const SizedBox(height: 10),
            const Text(
              'Hero',
              style: TextStyle(fontSize: 25),
            ),
            const SizedBox(height: 35),
            _buildTextFieldWith(_bokuIDTextController, 'Enter your Boku ID',
                currentAppState.getAppConnectionState),
            const SizedBox(height: 10),
            _buildTextFieldWith(_bokuPassTextController, 'Enter your Boku Pass',
                currentAppState.getAppConnectionState),
            const SizedBox(height: 10),
            _buildConnecteButtonFrom(currentAppState.getAppConnectionState),
            const SizedBox(height: 10),
            _buildPublishMessageRow(),
            const SizedBox(height: 10),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Column(
                  // mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    TextButton(
                      onPressed: () {},
                      child: Text("Restart"),
                    ),
                    TextButton(
                      onPressed: () {},
                      child: Text("Reset"),
                    ),
                  ],
                ),
                SizedBox(
                  width: 25,
                ),
                _buildLockButtonFrom(currentAppState.getAppConnectionState),
              ],
            ),
            const Text(
              'Courier',
              style: TextStyle(fontSize: 25),
            ),
            const SizedBox(height: 35),
          ],
        ),
      ),
    );
  }

  Widget _buildPublishMessageRow() {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
      children: <Widget>[
        Expanded(
          child: _buildTextFieldWith(_messageTextController, 'Add no.resi',
              currentAppState.getAppConnectionState),
        ),
        _buildSendButtonFrom(currentAppState.getAppConnectionState)
      ],
    );
  }

  Widget _buildConnectionStateText(String status) {
    return Row(
      children: <Widget>[
        Expanded(
          child: Container(
              color: Colors.deepOrangeAccent,
              child: Text(status, textAlign: TextAlign.center)),
        ),
      ],
    );
  }

  Widget _buildTextFieldWith(TextEditingController controller, String hintText,
      MQTTAppConnectionState state) {
    bool shouldEnable = false;
    if (controller == _messageTextController &&
        state == MQTTAppConnectionState.connected) {
      shouldEnable = true;
    } else if ((controller == _bokuIDTextController &&
            state == MQTTAppConnectionState.disconnected) ||
        (controller == _bokuPassTextController &&
            state == MQTTAppConnectionState.disconnected)) {
      shouldEnable = true;
    }
    return TextField(
        enabled: shouldEnable,
        controller: controller,
        decoration: InputDecoration(
          contentPadding:
              const EdgeInsets.only(left: 0, bottom: 0, top: 0, right: 0),
          labelText: hintText,
        ));
  }

  Widget _buildScrollableTextWith(String text) {
    return Padding(
      padding: const EdgeInsets.all(20.0),
      child: Container(
        width: 400,
        height: 200,
        child: SingleChildScrollView(
          child: Text(text),
        ),
      ),
    );
  }

  Widget _buildConnecteButtonFrom(MQTTAppConnectionState state) {
    return Row(
      children: <Widget>[
        Expanded(
          // ignore: deprecated_member_use
          child: RaisedButton(
            color: Colors.lightBlueAccent,
            child: const Text('Login'),
            onPressed: state == MQTTAppConnectionState.disconnected
                ? _configureAndConnect
                : null, //
          ),
        ),
        const SizedBox(width: 10),
        Expanded(
          // ignore: deprecated_member_use
          child: RaisedButton(
            color: Colors.redAccent,
            child: const Text('Logout'),
            onPressed: state == MQTTAppConnectionState.connected
                ? _disconnect
                : null, //
          ),
        ),
      ],
    );
  }

  Widget _buildSendButtonFrom(MQTTAppConnectionState state) {
    // ignore: deprecated_member_use
    return IconButton(
      icon: const Icon(
        Icons.add_box,
        size: 35,
        color: Colors.lightBlue,
      ),
      onPressed: state == MQTTAppConnectionState.connected
          ? () {
              _publishMessage(
                  '${_bokuPassTextController.text} add ${_messageTextController.text}');
            }
          : null,
    );

    // RaisedButton(
    //   color: Colors.green,
    //   child: const Text('Send'),
    //   onPressed: state == MQTTAppConnectionState.connected
    //       ? () {
    //           _publishMessage(
    //               '${_bokuPassTextController.text} add ${_messageTextController.text}');
    //         }
    //       : null, //
    // );
  }

  Widget _buildLockButtonFrom(MQTTAppConnectionState state) {
    // ignore: deprecated_member_use
    return IconButton(
      iconSize: 35,
      onPressed: state == MQTTAppConnectionState.connected
          ? () {
              isLock
                  ? _publishMessage('${_bokuPassTextController.text} open')
                  : _publishMessage('${_bokuPassTextController.text} close');
            }
          : null, //

      icon: isLock ? const Icon(Icons.lock) : const Icon(Icons.lock_open),
      // () => lock(),
    );
  }

  // Utility functions
  String _prepareStateMessageFrom(MQTTAppConnectionState state) {
    switch (state) {
      case MQTTAppConnectionState.connected:
        return 'Connected';
      case MQTTAppConnectionState.connecting:
        return 'Connecting';
      case MQTTAppConnectionState.disconnected:
        return 'Disconnected';
    }
  }

  void _configureAndConnect() {
    // ignore: flutter_style_todos
    // TODO: Use UUID
    String osPrefix = 'Flutter_iOS';
    if (Platform.isAndroid) {
      osPrefix = 'Flutter_Android';
    }
    final String _topic = 'Boku/${_bokuIDTextController.text}';
    manager = MQTTManager(
        host: 'broker.hivemq.com',
        topic: _topic,
        identifier: osPrefix,
        state: currentAppState);
    manager.initializeMQTTClient();
    manager.connect();
  }

  void _disconnect() {
    manager.disconnect();
  }

  void _publishMessage(String text) {
    // String osPrefix = 'Flutter_iOS';
    // if (Platform.isAndroid) {
    //   osPrefix = 'Flutter_Android';
    // }
    final String message = text;
    manager.publish(message);
    _messageTextController.clear();
  }
}
