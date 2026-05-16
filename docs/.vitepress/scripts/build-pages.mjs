import { spawnSync } from 'node:child_process'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const docsRoot = path.resolve(__dirname, '..', '..')
const vitepressCli = path.resolve(docsRoot, 'node_modules', 'vitepress', 'dist', 'node', 'cli.js')

const result = spawnSync(process.execPath, [vitepressCli, 'build'], {
  cwd: docsRoot,
  stdio: 'inherit',
  env: {
    ...process.env,
    VITEPRESS_BASE: '/cpp-high-performance-guide/',
  },
})

if (result.error) {
  throw result.error
}

process.exit(result.status ?? 1)
