import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'
import { createSSRApp, h } from 'vue'
import { renderToString } from 'vue/server-renderer'
import { compileScript, compileTemplate, parse } from '@vue/compiler-sfc'
import { transformWithEsbuild } from 'vite'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const themeDir = path.resolve(__dirname, '..', 'theme')
const vueModuleUrl = import.meta.resolve('vue')

function read(relativePath) {
  return fs.readFileSync(path.join(themeDir, relativePath), 'utf8')
}

function extractLandingArray(content, propName) {
  const match = content.match(new RegExp(`:${propName}='\\[(.*?)\\]'`, 's'))

  assert.ok(match, `expected LandingHero ${propName} array`)
  return match[1]
}

function extractLandingActions(content) {
  return [...extractLandingArray(content, 'actions').matchAll(/href:\s*"([^"]+)"/g)].map(([, href]) => href)
}

function extractLandingGuides(content) {
  return [...extractLandingArray(content, 'guides').matchAll(/href:\s*"([^"]+)"/g)].map(([, href]) => href)
}

function extractLandingMetricValues(content) {
  return [...extractLandingArray(content, 'metrics').matchAll(/value:\s*"([^"]+)"/g)].map(([, value]) => value)
}

function extractSectionIndexLinkGroups(content) {
  return [...content.matchAll(/<SectionIndex\b[\s\S]*?\/?>/g)].map(([block]) => {
    return [...block.matchAll(/href:\s*"([^"]+)"/g)].map(([, href]) => href)
  })
}

function countModuleMapRows(content, startHeading, endHeading) {
  const escapedStart = startHeading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')
  const escapedEnd = endHeading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')
  const sectionMatch = content.match(new RegExp(`## ${escapedStart}[\\s\\S]*?## ${escapedEnd}`))

  assert.ok(sectionMatch, `expected section between ${startHeading} and ${endHeading}`)

  return [...sectionMatch[0].matchAll(/^\| (?:0[1-9]|[1-9][0-9]*)\./gm)].length
}

function normalizeHeroLink(href) {
  if (href.startsWith('http')) {
    return href
  }

  return href.replace(/^\/(?:en|zh)(?=\/|$)/, '') || '/'
}

async function loadSfcComponent(relativePath) {
  const filename = path.join(themeDir, relativePath)
  const source = fs.readFileSync(filename, 'utf8')
  const { descriptor } = parse(source, { filename })
  const script = compileScript(descriptor, { id: relativePath })
  const template = compileTemplate({
    id: relativePath,
    filename,
    source: descriptor.template.content,
    compilerOptions: {
      bindingMetadata: script.bindings,
    },
  })

  const combined = [
    script.content.replace('export default', 'const __sfc__ ='),
    template.code,
    'const __component__ = __sfc__',
    '__component__.render = render',
    'export default __component__',
  ].join('\n')

  const transformed = await transformWithEsbuild(combined, filename, {
    loader: 'ts',
    format: 'esm',
    target: 'es2022',
  })

  const moduleSource = transformed.code.replace(/from\s+['"]vue['"]/g, `from '${vueModuleUrl}'`)
  return import(`data:text/javascript;charset=utf-8,${encodeURIComponent(moduleSource)}`)
}

test('style.css keeps tokenized selectors for the live homepage, Mermaid, and SVG surfaces', () => {
  const css = read('style.css')
  const colorTokens = read('tokens/colors.css')

  for (const token of [
    '--wp-paper-1',
    '--wp-ink-1',
    '--wp-surface-figure',
    '--wp-surface-meta',
    '--wp-surface-section-index',
    '--wp-surface-reference',
    '--wp-line-1',
    '--wp-surface-1',
    '--wp-surface-2',
    '--wp-pill-bg',
    '--wp-diagram-stroke',
    '--wp-icon-muted',
  ]) {
    assert.match(colorTokens, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const selector of [
    '.landing-hero',
    '.wp-section-index',
    '.wp-figure-shell',
    '.wp-meta-strip',
    '.wp-reference-list',
    '.vp-doc .mermaid',
    ".vp-doc :where(svg [stroke='currentColor']",
  ]) {
    assert.match(css, new RegExp(selector.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('style.css suppresses VitePress built-in translation widgets in favor of the custom switcher', () => {
  const css = read('style.css')

  for (const selector of [
    '.VPNavBarTranslations',
    '.VPNavBarExtra .translations',
    '.VPNavScreenTranslations',
  ]) {
    assert.match(css, new RegExp(selector.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('style.css includes dedicated nav-density selectors for the redesigned homepage', () => {
  const css = read('style.css')

  assert.match(css, /\.VPNavBarMenu\s*\{[\s\S]*?gap:\s*clamp\(0\.45rem,\s*1vw,\s*1rem\);/)
  assert.match(css, /\.VPNavBarMenuLink,[\s\S]*?\.VPNavBarMenuGroup \.button\s*\{[\s\S]*?font-size:\s*0\.94rem;/)
  assert.match(css, /\.VPNavBarExtra\s*\{[\s\S]*?gap:\s*0\.55rem;/)
  assert.match(css, /\.landing-hero__shell\s*\{/) 
})

test('language switcher markup keeps native navigation when JavaScript is unavailable', () => {
  const switcher = read('LanguageSwitcher.vue')

  assert.match(switcher, /<details\b/)
  assert.match(switcher, /<summary\b/)
  assert.match(switcher, /<ul\b/)
  assert.match(switcher, /<li\b/)
  assert.match(switcher, /<a\b/)
  assert.match(switcher, /:href=/)
  assert.doesNotMatch(switcher, /\brole="menu"/)
})

test('theme index wires only the active language chrome', () => {
  const themeIndex = read('index.ts')

  for (const componentName of ['BaseAwareLink', 'LanguageRedirect', 'LanguageSwitcher', 'LandingHero', 'SectionIndex']) {
    assert.match(themeIndex, new RegExp(componentName))
  }

  for (const componentName of ['SectionHero', 'MetricStrip']) {
    assert.doesNotMatch(themeIndex, new RegExp(componentName))
  }
})

test('bilingual landing pages preserve copy while using shared whitepaper primitives', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  for (const content of [enIndex, zhIndex]) {
    assert.match(content, /<LandingHero\b/)
    assert.match(content, /<SectionIndex\b/)
    assert.doesNotMatch(content, /<SectionHero\b/)
    assert.doesNotMatch(content, /<MetricStrip\b/)
    assert.doesNotMatch(content, /class="home-header"/)
    assert.doesNotMatch(content, /class="home-intro-row"/)
    assert.doesNotMatch(content, /class="feature-map"/)
  }

  assert.match(enIndex, /<BaseAwareLink href="\/en\/getting-started\/quickstart">Quick Start guide<\/BaseAwareLink>/)
  assert.match(zhIndex, /<BaseAwareLink href="\/zh\/getting-started\/quickstart">快速开始指南<\/BaseAwareLink>/)
  assert.match(enIndex, /:actions='/)
  assert.match(zhIndex, /:actions='/)
  assert.doesNotMatch(enIndex, /href: "\.\//)
  assert.doesNotMatch(enIndex, /href: "\.\.\//)
  assert.doesNotMatch(zhIndex, /href: "\.\//)
  assert.doesNotMatch(zhIndex, /href: "\.\.\//)
  assert.doesNotMatch(enIndex, /<a href="\/en\//)
  assert.doesNotMatch(zhIndex, /<a href="\/zh\//)
})

test('bilingual landing pages promote equivalent landing actions', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  assert.deepEqual(
    extractLandingActions(zhIndex).map(normalizeHeroLink),
    extractLandingActions(enIndex).map(normalizeHeroLink),
  )
})

test('bilingual landing pages keep equivalent guide routes and summary metrics', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  assert.deepEqual(
    extractLandingGuides(zhIndex).map(normalizeHeroLink),
    extractLandingGuides(enIndex).map(normalizeHeroLink),
  )

  assert.deepEqual(extractLandingMetricValues(zhIndex), extractLandingMetricValues(enIndex))
})

test('bilingual landing pages keep equivalent section index routes and module map rows', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  const enSectionIndexGroups = extractSectionIndexLinkGroups(enIndex)
  const zhSectionIndexGroups = extractSectionIndexLinkGroups(zhIndex)

  assert.equal(enSectionIndexGroups.length, 2)
  assert.equal(zhSectionIndexGroups.length, 2)
  assert.deepEqual(
    zhSectionIndexGroups.map(group => group.map(normalizeHeroLink)),
    enSectionIndexGroups.map(group => group.map(normalizeHeroLink)),
  )

  assert.equal(countModuleMapRows(enIndex, 'Module map', 'Expert reader callouts'), 5)
  assert.equal(countModuleMapRows(zhIndex, '模块地图', '面向熟练读者的提示'), 5)
})

test('LandingHero renders split hero props and base-aware links through shared link nodes', async () => {
  const { default: LandingHero } = await loadSfcComponent('LandingHero.vue')

  const app = createSSRApp({
    render() {
      return h(LandingHero, {
        badge: 'Updated for 2026',
        title: 'Learn modern C++',
        titleAccent: 'systematically',
        subtitle: 'A practical path from foundations to validation.',
        intro: 'Use the guided module sequence, then branch into reference material.',
        actionsAriaLabel: 'Primary learning actions',
        actions: [
          { href: '/en/getting-started/quickstart', label: 'Start here', primary: true },
          { href: '/en/reference/', label: 'Browse references' },
        ],
        guidesAriaLabel: 'Recommended guide sequence',
        guides: [
          {
            href: '/en/guides/learning-path',
            title: 'Learn the module sequence',
            description: 'Follow the staged reading order.',
          },
        ],
        metricsAriaLabel: 'Learning metrics',
        metrics: [
          { value: '12+', label: 'Core modules' },
          { value: '3', label: 'Practice tracks' },
        ],
      })
    },
  })

  app.component('BaseAwareLink', {
    props: {
      href: {
        type: String,
        required: true,
      },
    },
    setup(props, { slots, attrs }) {
      return () => h('a', { ...attrs, href: `resolved:${props.href}` }, slots.default?.())
    },
  })

  const html = await renderToString(app)

  assert.match(html, /Primary learning actions/)
  assert.match(html, /Recommended guide sequence/)
  assert.match(html, /Learning metrics/)
  assert.match(html, /Learn the module sequence/)
  assert.match(html, /href="resolved:\/en\/getting-started\/quickstart"/)
  assert.match(html, /href="resolved:\/en\/guides\/learning-path"/)
  assert.match(html, /class="landing-hero__guide-title"/)
})
