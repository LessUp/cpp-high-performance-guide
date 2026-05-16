import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'
import { createSSRApp } from 'vue'
import { renderToString } from 'vue/server-renderer'
import { compileScript, compileTemplate, parse } from '@vue/compiler-sfc'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const themeDir = path.resolve(__dirname, '..', 'theme')
const vueModuleUrl = import.meta.resolve('vue')

function read(relativePath) {
  return fs.readFileSync(path.join(themeDir, relativePath), 'utf8')
}

async function renderComponent(relativePath, props) {
  const source = read(relativePath)
  const { descriptor } = parse(source, { filename: relativePath })
  const script = compileScript(descriptor, {
    id: relativePath,
    inlineTemplate: false,
  })
  const template = compileTemplate({
    id: relativePath,
    source: descriptor.template.content,
    filename: relativePath,
  })
  const moduleSource = `${script.content
    .replace(/^type\s+\w+\s*=\s*\{[\s\S]*?^\}\n*/gm, '')
    .replace(/__props:\s*any/g, '__props')
    .replace('export default', 'const component =')}
${template.code.replace('export function render', 'function render')}
component.render = render
export default component`.replaceAll("'vue'", `'${vueModuleUrl}'`).replaceAll('"vue"', `"${vueModuleUrl}"`)
  const dataUrl = `data:text/javascript;base64,${Buffer.from(moduleSource).toString('base64')}`
  const { default: component } = await import(dataUrl)
  const app = createSSRApp(component, props)
  return renderToString(app)
}

test('style.css keeps tokenized selectors for the live homepage, Mermaid, and SVG surfaces', () => {
  const css = read('style.css')

  for (const token of [
    '--wp-paper-1',
    '--wp-ink-1',
    '--wp-panel-bg',
    '--wp-figure-bg',
    '--wp-meta-bg',
    '--wp-section-index-bg',
    '--wp-section-index-border',
    '--wp-surface-1',
    '--wp-surface-2',
    '--wp-surface-section-index',
    '--wp-pill-bg',
    '--wp-diagram-stroke',
    '--wp-icon-muted',
  ]) {
    assert.match(css, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const selector of [
    '.wp-hero',
    '.wp-metric-strip',
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

  for (const componentName of ['LanguageRedirect', 'LanguageSwitcher', 'SectionHero', 'MetricStrip', 'SectionIndex']) {
    assert.match(themeIndex, new RegExp(componentName))
  }
})

test('bilingual landing pages preserve copy while using shared whitepaper primitives', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  for (const content of [enIndex, zhIndex]) {
    assert.match(content, /<SectionHero\b/)
    assert.match(content, /<MetricStrip\b/)
    assert.match(content, /<SectionIndex\b/)
    assert.doesNotMatch(content, /class="home-header"/)
    assert.doesNotMatch(content, /class="home-intro-row"/)
    assert.doesNotMatch(content, /class="feature-map"/)
  }

  assert.match(enIndex, /title="C\+\+ High Performance Guide"/)
  assert.match(enIndex, /A practical C\+\+20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling\./)
  assert.match(enIndex, /title="Quick Start"/)
  assert.match(zhIndex, /title="C\+\+ 高性能指南"/)
  assert.match(zhIndex, /一份实用的 C\+\+20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。/)
  assert.match(zhIndex, /title="快速开始"/)
  assert.match(enIndex, /links-aria-label="Landing page links"/)
  assert.match(enIndex, /aria-label="Project metrics"/)
  assert.match(zhIndex, /links-aria-label="落地页链接"/)
  assert.match(zhIndex, /aria-label="项目指标"/)

  for (const href of [
    '/en/getting-started/quickstart',
    '/en/guides/learning-path',
    '/en/exercises/module-02-memory',
    '/en/exercises/module-04-simd',
    '/en/exercises/module-05-concurrency',
    '/en/getting-started/prerequisites',
    '/en/guides/profiling-guide',
    '/en/guides/optimization-decision-tree',
    '/en/guides/validation',
    '/en/guides/best-practices',
    '/zh/',
  ]) {
    assert.match(enIndex, new RegExp(`href: "${href.replaceAll('/', '\\/')}"`))
  }

  for (const href of [
    '/zh/getting-started/quickstart',
    '/zh/guides/learning-path',
    '/en/exercises/module-02-memory',
    '/en/exercises/module-04-simd',
    '/en/exercises/module-05-concurrency',
    '/zh/getting-started/prerequisites',
    '/zh/guides/profiling-guide',
    '/zh/guides/optimization-decision-tree',
    '/zh/guides/validation',
    '/zh/guides/best-practices',
    '/en/',
  ]) {
    assert.match(zhIndex, new RegExp(`href: "${href.replaceAll('/', '\\/')}"`))
  }

  assert.match(enIndex, /<a href="\/en\/getting-started\/quickstart">Quick Start guide<\/a>/)
  assert.match(zhIndex, /<a href="\/zh\/getting-started\/quickstart">快速开始指南<\/a>/)
  assert.doesNotMatch(enIndex, /href: "\.\//)
  assert.doesNotMatch(enIndex, /href: "\.\.\//)
  assert.doesNotMatch(zhIndex, /href: "\.\//)
  assert.doesNotMatch(zhIndex, /href: "\.\.\//)
  assert.doesNotMatch(enIndex, /<a href="\.\//)
  assert.doesNotMatch(zhIndex, /<a href="\.\//)
})

test('shared landing components render localized aria labels', async () => {
  const heroHtml = await renderComponent('SectionHero.vue', {
    title: 'C++ 高性能指南',
    intro: '一份实用的 C++20 指南。',
    linksAriaLabel: '落地页链接',
    links: [{ href: '../en/', label: 'English' }],
  })
  const metricHtml = await renderComponent('MetricStrip.vue', {
    ariaLabel: '项目指标',
    items: [{ value: '双语', label: '文档' }],
  })

  assert.match(heroHtml, /aria-label="落地页链接"/)
  assert.doesNotMatch(heroHtml, /aria-label="Landing page links"/)
  assert.match(metricHtml, /<section\b/)
  assert.match(metricHtml, /aria-label="项目指标"/)
  assert.doesNotMatch(metricHtml, /aria-label="Project metrics"/)
})
