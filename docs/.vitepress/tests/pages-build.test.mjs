import test from 'node:test'
import assert from 'node:assert/strict'
import { spawnSync } from 'node:child_process'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const packageJsonPath = path.resolve(__dirname, '..', '..', 'package.json')
const buildPagesScriptPath = path.resolve(__dirname, '..', 'scripts', 'build-pages.mjs')
const configPath = path.resolve(__dirname, '..', 'config.ts')
const docsRoot = path.resolve(__dirname, '..', '..')
const distRoot = path.resolve(docsRoot, '.vitepress', 'dist')

const cachedLandingPages = new Map()

function extractLandingHeroSection(html) {
  const heroMatch = html.match(/<section class="landing-hero"[\s\S]*?<\/section>/)
  assert.ok(heroMatch, 'expected built homepage LandingHero section')
  return heroMatch[0]
}

function readBuiltLandingPages(base = '/cpp-high-performance-guide/') {
  if (!cachedLandingPages.has(base)) {
    const result = spawnSync(process.execPath, [buildPagesScriptPath], {
      cwd: docsRoot,
      encoding: 'utf8',
      env: {
        ...process.env,
        VITEPRESS_BASE: base,
      },
    })

    assert.equal(result.status, 0, result.stderr || result.stdout || 'build:pages failed')

    cachedLandingPages.set(base, {
      en: fs.readFileSync(path.join(distRoot, 'en', 'index.html'), 'utf8'),
      zh: fs.readFileSync(path.join(distRoot, 'zh', 'index.html'), 'utf8'),
      exercisesIndexExists: fs.existsSync(path.join(distRoot, 'en', 'exercises', 'index.html')),
      exercisesReadmeExists: fs.existsSync(path.join(distRoot, 'en', 'exercises', 'README.html')),
    })
  }

  return cachedLandingPages.get(base)
}

test('docs config keeps clean URLs disabled for static hosting', () => {
  const config = fs.readFileSync(configPath, 'utf8')

  assert.match(config, /cleanUrls:\s*false/)
  assert.doesNotMatch(config, /cleanUrls:\s*true/)
})

test('build:pages uses a cross-platform Node wrapper while preserving external base paths', () => {
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'))
  const buildPagesScript = fs.readFileSync(buildPagesScriptPath, 'utf8')

  assert.equal(packageJson.scripts['build:pages'], 'node .vitepress/scripts/build-pages.mjs')
  assert.match(buildPagesScript, /VITEPRESS_BASE/)
  assert.match(buildPagesScript, /\/cpp-high-performance-guide\//)
  assert.match(buildPagesScript, /vitepress/)
  assert.match(buildPagesScript, /process\.env\.VITEPRESS_BASE\s*\|\|/)
})

test('build:pages prefixes homepage landing links with the GitHub Pages base and .html leaf routes', () => {
  const { en, zh } = readBuiltLandingPages()
  const enHero = extractLandingHeroSection(en)
  const zhHero = extractLandingHeroSection(zh)

  for (const href of [
    '/cpp-high-performance-guide/en/reference/',
    '/cpp-high-performance-guide/en/getting-started/quickstart.html',
    '/cpp-high-performance-guide/en/guides/learning-path.html',
    '/cpp-high-performance-guide/en/guides/validation.html',
    '/cpp-high-performance-guide/en/contributing/ai-workflow.html',
    '/cpp-high-performance-guide/zh/',
  ]) {
    assert.match(en, new RegExp(`href="${href.replaceAll('/', '\\/')}"`))
  }

  for (const href of [
    '/cpp-high-performance-guide/zh/reference/',
    '/cpp-high-performance-guide/zh/getting-started/quickstart.html',
    '/cpp-high-performance-guide/zh/guides/learning-path.html',
    '/cpp-high-performance-guide/zh/guides/validation.html',
    '/cpp-high-performance-guide/zh/contributing/ai-workflow.html',
    '/cpp-high-performance-guide/en/',
  ]) {
    assert.match(zh, new RegExp(`href="${href.replaceAll('/', '\\/')}"`))
  }

  assert.match(enHero, /href="\/cpp-high-performance-guide\/en\/getting-started\/quickstart(?:\.html)?"/)
  assert.match(enHero, /href="\/cpp-high-performance-guide\/en\/academy\/"/)
  assert.match(enHero, /href="\/cpp-high-performance-guide\/en\/academy\/module-atlas(?:\.html)?"/)
  assert.match(enHero, /href="\/cpp-high-performance-guide\/en\/academy\/validation-doctrine(?:\.html)?"/)
  assert.match(enHero, /href="\/cpp-high-performance-guide\/en\/architecture\/performance-methodology(?:\.html)?"/)
  assert.doesNotMatch(enHero, /href="\/(?:en|zh)\//)

  assert.match(zhHero, /href="\/cpp-high-performance-guide\/zh\/getting-started\/quickstart(?:\.html)?"/)
  assert.match(zhHero, /href="\/cpp-high-performance-guide\/zh\/academy\/"/)
  assert.match(zhHero, /href="\/cpp-high-performance-guide\/zh\/academy\/module-atlas(?:\.html)?"/)
  assert.match(zhHero, /href="\/cpp-high-performance-guide\/zh\/academy\/validation-doctrine(?:\.html)?"/)
  assert.match(zhHero, /href="\/cpp-high-performance-guide\/zh\/architecture\/performance-methodology(?:\.html)?"/)
  assert.doesNotMatch(zhHero, /href="\/(?:en|zh)\//)

  for (const rawHref of [
    'href="/en/reference/"',
    'href="/en/getting-started/quickstart"',
    'href="/en/guides/learning-path"',
    'href="/en/guides/validation"',
    'href="/en/contributing/ai-workflow"',
    'href="/zh/"',
  ]) {
    assert.doesNotMatch(en, new RegExp(rawHref.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const rawHref of [
    'href="/zh/reference/"',
    'href="/zh/getting-started/quickstart"',
    'href="/zh/guides/learning-path"',
    'href="/zh/guides/validation"',
    'href="/zh/contributing/ai-workflow"',
    'href="/en/"',
  ]) {
    assert.doesNotMatch(zh, new RegExp(rawHref.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('build:pages preserves an externally supplied base path', () => {
  const { en, zh } = readBuiltLandingPages('/custom-base/')

  assert.match(en, /href="\/custom-base\/en\/getting-started\/quickstart\.html"/)
  assert.match(en, /href="\/custom-base\/zh\/"/)
  assert.doesNotMatch(en, /href="\/cpp-high-performance-guide\/en\/getting-started\/quickstart\.html"/)

  assert.match(zh, /href="\/custom-base\/zh\/getting-started\/quickstart\.html"/)
  assert.match(zh, /href="\/custom-base\/en\/"/)
  assert.doesNotMatch(zh, /href="\/cpp-high-performance-guide\/zh\/getting-started\/quickstart\.html"/)
})

test('build:pages emits the exercises overview as a clean index route', () => {
  const { exercisesIndexExists, exercisesReadmeExists } = readBuiltLandingPages()

  assert.equal(exercisesIndexExists, true)
  assert.equal(exercisesReadmeExists, false)
})
