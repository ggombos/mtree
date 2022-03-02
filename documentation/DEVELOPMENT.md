# Development of mtree_gist

## Visual Studio Code

The recommended IDE for developing `mtree_gist` is `Visual Studio Code`.

### Extensions

There are some recommended extensions:

- [Better C++ Syntax](https://marketplace.visualstudio.com/items?itemName=jeff-hykin.better-cpp-syntax)
- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [C/C++ Themes](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-themes)
- [ShellCheck](https://marketplace.visualstudio.com/items?itemName=timonwong.shellcheck)

### c_cpp_properties.json

You will also need a similar `c_cpp_properties.json` file as well. Note, that the `includePath` property should contain the path to the `PostgreSQL` include headers.

```json
{
  "configurations": [
    {
      "name": "Linux",
      "intelliSenseMode": "gcc-x64",
      "compilerPath": "/usr/bin/gcc",
      "includePath": [
        "${workspaceFolder}/**",
        "/usr/include/postgresql/**"
      ],
      "cStandard": "c99",
      "cppStandard": "c++17"
    }
  ],
  "version": 4
}
```
