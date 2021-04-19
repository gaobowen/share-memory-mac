
if (process.platform !== 'darwin') {
  throw new Error('Only works on macOS.')
} else {
  const addon = require('bindings')('share_memory_mac')
  module.exports = addon
}