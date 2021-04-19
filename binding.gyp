{
  "targets": [
    {
      "target_name": "share_memory_mac",
      "sources": [ ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
      ],
      "conditions": [
        ['OS=="mac"', {
          "sources": [ "src/share-memory-mac.cc", "src/addon.cc" ],
        }]
      ]
    }
  ]
}