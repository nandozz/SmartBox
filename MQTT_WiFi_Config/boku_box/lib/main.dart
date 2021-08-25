import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'pages/loginpage.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  // const MyApp({ Key? key }) : super(key: key);

  @override
  Widget build(BuildContext context) {
// Size size = MediaQuery.of(context).size;
    return GetMaterialApp(
      debugShowCheckedModeBanner: false,
      home: LoginPage(),
      getPages: [
        // GetPage(name: '/welcome', page: () => LoginPage()),
        GetPage(name: '/login', page: () => LoginPage()),
        // GetPage(name: '/signup', page: () => LoginPage()),
        // GetPage(name: '/catalog', page: () => CatalogPage()),
        // GetPage(name: '/detail/:categories?', page: () => DetailItem()),
        // GetPage(name: '/addItem/:categories?', page: () => AddItems()),
      ],
      theme: ThemeData(
        // textTheme: GoogleFonts.mulishTextTheme(Theme.of(context).textTheme),
        elevatedButtonTheme: ElevatedButtonThemeData(
          style: ElevatedButton.styleFrom(
              primary: Color.fromRGBO(252, 230, 30, 0.75),
              shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(50)),
              textStyle: TextStyle(
                  fontSize: 20,
                  fontWeight: FontWeight.bold,
                  color: Colors.purple)),
        ),
      ),
    );
  }
}
