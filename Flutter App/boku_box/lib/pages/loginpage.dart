import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:lottie/lottie.dart';
import 'package:flutter_barcode_scanner/flutter_barcode_scanner.dart';
import 'package:flutter/services.dart';
import 'dart:async';

// var _scanBarcode = ''.obs;
var count = 1.obs;
var islock = false.obs;
void lock() {
  // islock.value = !islock;
  islock.value = !islock.value;
  count++;
}

final Rx<TextEditingController> bokuID = TextEditingController().obs;
final Rx<TextEditingController> bokuPass = TextEditingController().obs;
final Rx<TextEditingController> addResi = TextEditingController().obs;
final Rx<TextEditingController> resiNo = TextEditingController().obs;

String publish = "";

// ignore: must_be_immutable
class LoginPage extends StatelessWidget {
  // const HomePages({Key? key}) : super(key: key);
  // var islock = false.obs;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Padding(
          padding: EdgeInsets.all(16),
          child: SingleChildScrollView(
            child: Center(
              child: Obx(
                () => Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Container(
                      child: Column(
                        children: [
                          Text(
                            "Hero",
                            style: TextStyle(fontSize: 25),
                          ),
                          TextField(
                            controller: bokuID.value,
                            style:
                                TextStyle(fontSize: 18, color: Colors.black54),
                            decoration: InputDecoration(
                              labelText: "Boku ID",
                              labelStyle: TextStyle(color: Colors.amber),
                            ),
                          ),
                          TextField(
                            controller: bokuPass.value,
                            style:
                                TextStyle(fontSize: 18, color: Colors.black54),
                            decoration: InputDecoration(
                              labelText: "Boku Pass",
                              labelStyle: TextStyle(color: Colors.amber),
                            ),
                          ),
                          TextField(
                            controller: addResi.value,
                            style:
                                TextStyle(fontSize: 18, color: Colors.black54),
                            decoration: InputDecoration(
                              labelText: "Add No.Resi",
                              labelStyle: TextStyle(color: Colors.amber),
                            ),
                          ),
                          SizedBox(height: 15),
                          Row(
                            mainAxisAlignment: MainAxisAlignment.spaceAround,
                            children: [
                              IconButton(
                                iconSize: 35,
                                onPressed: () => lock(),
                                icon: islock.value
                                    ? Icon(Icons.lock)
                                    : Icon(Icons.lock_open),
                              ),
                              TextButton.icon(
                                onPressed: () {},
                                icon: Icon(Icons.send),
                                label: Text("Hero"),
                              ),
                            ],
                          ),
                        ],
                      ),
                    ),
                    ////////////////////////////////////////////////////////////////////////////////////////////////
                    SizedBox(height: 45),
                    Lottie.asset(
                      "assets/lottie/walkingbox.json",
                      // "https://assets7.lottiefiles.com/packages/lf20_i7bmwsni.json",
                      width: 75,
                      height: 75,
                      fit: BoxFit.cover,
                      errorBuilder: (BuildContext context, Object exception,
                          StackTrace stackTrace) {
                        return Text('Your error widget...');
                      },
                    ),
                    ///////////////////////////////////// COURIER /////////////////////////////////////////////////

                    SizedBox(height: 45),
                    Container(
                      child: Column(
                        children: [
                          Text(
                            "Courier",
                            style: TextStyle(fontSize: 25),
                          ),
                          SizedBox(
                            height: 15,
                          ),
                          SizedBox(height: 15),
                          TextField(
                            controller: bokuID.value,
                            style:
                                TextStyle(fontSize: 18, color: Colors.black54),
                            decoration: InputDecoration(
                              labelText: "Boku ID",
                              labelStyle: TextStyle(color: Colors.amber),
                              hintText: "Ketik/scan Boku ID",
                              hintStyle: TextStyle(
                                  color: Colors.lightBlue.withOpacity(.5)),
                              suffixIcon: IconButton(
                                onPressed: () {
                                  scanQR(1);
                                },
                                icon: Icon(
                                  Icons.camera_alt,
                                  size: 35,
                                  color: Colors.lightBlue,
                                ),
                              ),
                            ),
                          ),
                          TextField(
                            controller: resiNo.value,
                            style:
                                TextStyle(fontSize: 18, color: Colors.black54),
                            decoration: InputDecoration(
                              labelText: "No. Resi",
                              labelStyle: TextStyle(color: Colors.amber),
                              hintText: "Ketik/scan No.Resi paket",
                              hintStyle: TextStyle(
                                  color: Colors.lightBlue.withOpacity(.5)),
                              suffixIcon: IconButton(
                                onPressed: () {
                                  scanQR(2);
                                },
                                icon: Icon(
                                  Icons.camera_alt,
                                  size: 35,
                                  color: Colors.lightBlue,
                                ),
                              ),
                            ),
                          ),
                          SizedBox(height: 35),
                          TextButton.icon(
                            onPressed: () {
                              bokuID.value.text = "";
                              resiNo.value.text = "";
                            },
                            icon: Icon(Icons.send),
                            label: Text("Courier"),
                          ),
                        ],
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }

  Future<void> scanQR(int mode) async {
    String barcodeScanRes;
    // Platform messages may fail, so we use a try/catch PlatformException.
    try {
      barcodeScanRes = mode == 1
          ? await FlutterBarcodeScanner.scanBarcode(
              '#ff6666', 'Cancel', true, ScanMode.QR)
          : await FlutterBarcodeScanner.scanBarcode(
              '#ff6666', 'Cancel', true, ScanMode.BARCODE);
      if (barcodeScanRes == "-1") {
        barcodeScanRes = 'Failed to get the code';
      }
      print(barcodeScanRes);
    } on PlatformException {
      barcodeScanRes = 'Failed to get platform version.';
    }
    mode == 1
        ? bokuID.value.text = barcodeScanRes
        : resiNo.value.text = barcodeScanRes;
  }
}
