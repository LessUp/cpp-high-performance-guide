import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const themeDir = path.resolve(__dirname, '..', 'theme')

function read(relativePath) {
  return fs.readFileSync(path.join(themeDir, relativePath), 'utf8')
}

function readAllCss() {
  const mainCss = read('style.css')
  const tokensDir = path.join(themeDir, 'tokens')
  let allCss = mainCss

  if (fs.existsSync(tokensDir)) {
    const tokenFiles = fs.readdirSync(tokensDir).filter(file => file.endsWith('.css'))
    for (const file of tokenFiles) {
      allCss += `\n${fs.readFileSync(path.join(tokensDir, file), 'utf8')}`
    }
  }

  return allCss
}

test('style.css keeps tokenized selectors for Mermaid and SVG surfaces', () => {
  const css = readAllCss()

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
    assert.match(css, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const selector of [
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

test('theme index registers the active custom components', () => {
  const themeIndex = read('index.ts')

  for (const componentName of ['BaseAwareLink', 'SectionIndex', 'Citation', 'ComplexityBadge', 'DiagramCanvas']) {
    assert.match(themeIndex, new RegExp(componentName))
  }
})
