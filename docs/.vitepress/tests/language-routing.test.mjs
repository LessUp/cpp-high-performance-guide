import test from 'node:test'
import assert from 'node:assert/strict'

import { createLanguageSwitcherLinks, resolveLanguageTarget } from '../theme/language-routing.js'

test('switching from no-trailing-slash Pages root to zh keeps a single base segment', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide',
    siteBase: '/cpp-high-performance-guide/',
    currentLangPath: '/en/',
    targetLangPath: '/zh/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/zh/')
})

test('switching from no-trailing-slash Pages root to en keeps a single base segment', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide',
    siteBase: '/cpp-high-performance-guide/',
    currentLangPath: '/zh/',
    targetLangPath: '/en/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/en/')
})

test('switching from an English-only API detail page to zh falls back to the zh API hub', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide/en/reference/api/memory-utils',
    siteBase: '/cpp-high-performance-guide/',
    targetLangPath: '/zh/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/zh/reference/api-reference')
})

test('switching from an English-only exercise page to zh falls back to the zh landing page', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide/en/exercises/module-02-memory',
    siteBase: '/cpp-high-performance-guide/',
    targetLangPath: '/zh/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/zh/')
})

test('language switcher exposes real href targets for each locale', () => {
  const links = createLanguageSwitcherLinks({
    routePath: '/cpp-high-performance-guide/en/guides/validation',
    siteBase: '/cpp-high-performance-guide/',
  })

  assert.deepEqual(links, [
    {
      code: 'en',
      label: 'English',
      path: '/en/',
      targetPath: '/cpp-high-performance-guide/en/guides/validation',
      isCurrent: true,
    },
    {
      code: 'zh',
      label: '中文',
      path: '/zh/',
      targetPath: '/cpp-high-performance-guide/zh/guides/validation',
      isCurrent: false,
    },
  ])
})

test('language switcher hrefs reuse locale fallbacks for untranslated destinations', () => {
  const links = createLanguageSwitcherLinks({
    routePath: '/cpp-high-performance-guide/en/reference/api/memory-utils',
    siteBase: '/cpp-high-performance-guide/',
  })

  assert.deepEqual(links, [
    {
      code: 'en',
      label: 'English',
      path: '/en/',
      targetPath: '/cpp-high-performance-guide/en/reference/api/memory-utils',
      isCurrent: true,
    },
    {
      code: 'zh',
      label: '中文',
      path: '/zh/',
      targetPath: '/cpp-high-performance-guide/zh/reference/api-reference',
      isCurrent: false,
    },
  ])
})
