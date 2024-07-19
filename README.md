<div align="center">

  <h3><b>Pump COntrol</b></h3>

</div>

# 📗 Table of Contents

- [📗 Table of Contents](#-table-of-contents)
- [📖 Embedded Systems Development ](#-pumcontrol-)
  - [🛠  Built With ](#--built-with-)
    - [Key Features ](#key-features-)
  - [💻 Getting Started ](#-getting-started-)
    - [Prerequisites](#prerequisites)
    - [Setup](#setup)
    - [Usage](#usage)
  - [👥 Authors ](#-authors-)
  - [🔭 Future Features ](#-future-features-)
  - [🤝 Contributing ](#-contributing-)
  - [⭐️ Show your support ](#️-show-your-support-)
  - [🙏 Acknowledgments ](#-acknowledgments-)
  - [❓ FAQ ](#-faq-)
  - [📝 License ](#-license-)


# 📖 Embedded Systems Development <a name="pumcontrol"></a>

### Pump Control is a robust embedded system project focused on efficient and secure data communication between users and IoT devices. This project involves handling structured data serialization and deserialization, real-time data exchange via WebSockets, and secure authentication mechanisms. Originally created for controlling a water pump, the code is versatile and can be adapted for various IoT device control systems.

## 🛠  Built With <a name="built-with"></a>
- C/C++
- JavaScript
- HTML
- CSS
- [FreeRTOS](https://www.freertos.org/)
- [Protocol buffer](https://protobuf.dev/) 
- [PlatformIO](https://platformio.org/)
- [Nanopb](https://github.com/nanopb/nanopb)
- [ESPAsyncWebServer](https://github.com/lacamera/ESPAsyncWebServer)

### Key Features <a name="key-features"></a>

- Efficient Data Serialization/Deserialization: Utilizing nanopb and protocol buffer for handling structured data.
- WebSocket Communication: Managing real-time data exchange.
- Memory Management: Ensuring proper management of dynamically allocated memory to prevent leaks.
- Testing: Comprehensive tests to ensure data integrity.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## 💻 Getting Started <a name="getting-started"></a>

### Prerequisites

- PlatformIO
- An ESP32 development board
- A Linux (e.g., Ubuntu) or windows environment

### Setup

Clone this repository to your desired folder:
```sh
    git clone https://github.com/sagieramos/pumpcontrol $HOME/pumpcontrol
```
### Usage

To run the project, execute the following command:

```sh
  cd $HOME/pumpcontrol
  pio run
  pio test -e test
  pio run -e esp32 -t upload
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## 👥 Authors <a name="authors"></a>
👤 **Stanley Osagie Ramos**
- GitHub: [@sagieramos](https://github.com/sagieramos)
- Twitter: [@sagieramos](https://twitter.com/sagieramos)
- LinkedIn: [LinkedIn](https://linkedin.com/in/sagieramos)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## 🔭 Future Features <a name="future-features"></a>

- [ ] **Add responsive layouts**
- [ ] **Implement additional security features**
- [ ] **Creating a communtication broker that interface clients and IoT devices**

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## 🤝 Contributing <a name="contributing"></a>

Contributions, issues, and feature requests are welcome!

Feel free to check the [issues page](https://github.com/sagieramos/pumpcontrol/issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



## ⭐️ Show your support <a name="support"></a>

If you like this project, please consider subscribing to my GitHub account for updates and more projects

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## 🙏 Acknowledgments <a name="acknowledgements"></a>
- Jehovah God
- [ALX Africa](https://www.alxafrica.com/).
- [Microverse Team](https://www.microverse.org/).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## ❓ FAQ <a name="faq"></a>

- **Can I run this project run on Docker?**

  - No, the project can only run on ESP32 for now.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## 📝 License <a name="license"></a>

This project is [MIT](./LICENSE) licensed.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- https://www.iconpacks.net/free-icon/warning-sign-9773.html -->