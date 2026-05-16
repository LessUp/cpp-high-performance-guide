import test from 'node:test'
import assert from 'node:assert/strict'
import { spawnSync } from 'node:child_process'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const packageJsonPath = path.resolve(__dirname, '..', '..', 'package.json')
const buildPagesScriptPath = path.resolve(__dirname, '..', 'scripts', 'build-pages.mjs')
const docsRoot = path.resolve(__dirname, '..', '..')
const distRoot = path.resolve(docsRoot, '.vitepress', 'dist')

let cachedLandingPages

function readBuiltLandingPages() {
  if (!cachedLandingPages) {
    const result = spawnSync(process.execPath, [buildPagesScriptPath], {
      cwd: docsRoot,
      encoding: 'utf8',
    })

    assert.equal(result.status, 0, result.stderr || result.stdout || 'build:pages failed')

    cachedLandingPages = {
      en: fs.readFileSync(path.join(distRoot, 'en', 'index.html'), 'utf8'),
      zh: fs.readFileSync(path.join(distRoot, 'zh', 'index.html'), 'utf8'),
    }
  }

  return cachedLandingPages
}

test('build:pages uses a cross-platform Node wrapper for the GitHub Pages base path', () => {
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'))
  const buildPagesScript = fs.readFileSync(buildPagesScriptPath, 'utf8')

  assert.equal(packageJson.scripts['build:pages'], 'node .vitepress/scripts/build-pages.mjs')
  assert.match(buildPagesScript, /VITEPRESS_BASE/)
  assert.match(buildPagesScript, /\/cpp-high-performance-guide\//)
  assert.match(buildPagesScript, /vitepress/)
})

test('build:pages prefixes homepage landing links with the GitHub Pages base', () => {
  const { en, zh } = readBuiltLandingPages()

  for (const href of [
    '/cpp-high-performance-guide/en/getting-started/quickstart',
    '/cpp-high-performance-guide/en/guides/learning-path',
    '/cpp-high-performance-guide/en/exercises/module-02-memory',
    '/cpp-high-performance-guide/en/exercises/module-04-simd',
    '/cpp-high-performance-guide/en/exercises/module-05-concurrency',
    '/cpp-high-performance-guide/en/getting-started/prerequisites',
    '/cpp-high-performance-guide/en/guides/profiling-guide',
    '/cpp-high-performance-guide/en/guides/optimization-decision-tree',
    '/cpp-high-performance-guide/en/guides/validation',
    '/cpp-high-performance-guide/en/guides/best-practices',
    '/cpp-high-performance-guide/zh/',
  ]) {
    assert.match(en, new RegExp(`href="${href.replaceAll('/', '\\/')}"`))
  }

  for (const href of [
    '/cpp-high-performance-guide/zh/getting-started/quickstart',
    '/cpp-high-performance-guide/zh/guides/learning-path',
    '/cpp-high-performance-guide/en/exercises/module-02-memory',
    '/cpp-high-performance-guide/en/exercises/module-04-simd',
    '/cpp-high-performance-guide/en/exercises/module-05-concurrency',
    '/cpp-high-performance-guide/zh/getting-started/prerequisites',
    '/cpp-high-performance-guide/zh/guides/profiling-guide',
    '/cpp-high-performance-guide/zh/guides/optimization-decision-tree',
    '/cpp-high-performance-guide/zh/guides/validation',
    '/cpp-high-performance-guide/zh/guides/best-practices',
    '/cpp-high-performance-guide/en/',
  ]) {
    assert.match(zh, new RegExp(`href="${href.replaceAll('/', '\\/')}"`))
  }

  for (const rawHref of [
    'href="/en/getting-started/quickstart"',
    'href="/en/guides/learning-path"',
    'href="/en/exercises/module-02-memory"',
    'href="/en/exercises/module-04-simd"',
    'href="/en/exercises/module-05-concurrency"',
    'href="/en/getting-started/prerequisites"',
    'href="/en/guides/profiling-guide"',
    'href="/en/guides/optimization-decision-tree"',
    'href="/en/guides/validation"',
    'href="/en/guides/best-practices"',
    'href="/zh/"',
  ]) {
    assert.doesNotMatch(en, new RegExp(rawHref.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const rawHref of [
    'href="/zh/getting-started/quickstart"',
    'href="/zh/guides/learning-path"',
    'href="/zh/getting-started/prerequisites"',
    'href="/zh/guides/profiling-guide"',
    'href="/zh/guides/optimization-decision-tree"',
    'href="/zh/guides/validation"',
    'href="/zh/guides/best-practices"',
    'href="/en/"',
    'href="/en/exercises/module-02-memory"',
    'href="/en/exercises/module-04-simd"',
    'href="/en/exercises/module-05-concurrency"',
  ]) {
    assert.doesNotMatch(zh, new RegExp(rawHref.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})
