import test from 'node:test'
import assert from 'node:assert/strict'

import {
  createLanguageSwitcherLinks,
  resolveCurrentLanguage,
  resolveLanguageTarget,
} from '../theme/language-routing.js'

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

test('slashless zh locale root is detected correctly under a Pages base', () => {
  const currentLanguage = resolveCurrentLanguage('/cpp-high-performance-guide/zh', '/cpp-high-performance-guide/')

  assert.equal(currentLanguage.code, 'zh')
})

test('slashless en locale root is detected correctly under a Pages base', () => {
  const currentLanguage = resolveCurrentLanguage('/cpp-high-performance-guide/en', '/cpp-high-performance-guide/')

  assert.equal(currentLanguage.code, 'en')
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

test('switching from a slashless English-only exercises index to zh falls back to the zh landing page', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide/en/exercises',
    siteBase: '/cpp-high-performance-guide/',
    targetLangPath: '/zh/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/zh/')
})

test('switching from a slashless English-only API index to zh falls back to the zh API hub', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide/en/reference/api',
    siteBase: '/cpp-high-performance-guide/',
    targetLangPath: '/zh/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/zh/reference/api-reference')
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

test('language switcher hrefs reuse locale fallbacks for slashless clean URLs under a Pages base', () => {
  const exerciseLinks = createLanguageSwitcherLinks({
    routePath: '/cpp-high-performance-guide/en/exercises',
    siteBase: '/cpp-high-performance-guide/',
  })
  const apiLinks = createLanguageSwitcherLinks({
    routePath: '/cpp-high-performance-guide/en/reference/api',
    siteBase: '/cpp-high-performance-guide/',
  })

  assert.equal(exerciseLinks[1].targetPath, '/cpp-high-performance-guide/zh/')
  assert.equal(apiLinks[1].targetPath, '/cpp-high-performance-guide/zh/reference/api-reference')
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
