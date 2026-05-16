<script setup lang="ts">
import { onMounted } from 'vue'
import { useData, useRoute, useRouter } from 'vitepress'
import { SUPPORTED_LANGS, resolveLanguageTarget, stripBase } from './language-routing.js'

const STORAGE_KEY = 'cpp-hpc-guide-lang-preference'
const DEFAULT_LANG = SUPPORTED_LANGS[0]?.code ?? 'en'

const router = useRouter()
const route = useRoute()
const { site } = useData()

function browserLocales(): string[] {
  if (typeof navigator === 'undefined') {
    return [DEFAULT_LANG]
  }

  const locales = navigator.languages?.length ? navigator.languages : [navigator.language]
  return locales.filter(Boolean)
}

function detectLanguage(): string {
  const stored = typeof localStorage !== 'undefined' ? localStorage.getItem(STORAGE_KEY) : null
  if (stored && SUPPORTED_LANGS.some(lang => lang.code === stored)) {
    return stored
  }

  for (const locale of browserLocales()) {
    const normalizedLocale = locale.toLowerCase()
    const match = SUPPORTED_LANGS.find(lang => normalizedLocale.startsWith(lang.code))
    if (match) {
      return match.code
    }
  }

  return DEFAULT_LANG
}

function normalizePath(path: string): string {
  const normalized = path.replace(/\/+$/, '')
  return normalized || '/'
}

onMounted(() => {
  if (typeof window === 'undefined') {
    return
  }

  const siteBase = site.value.base || '/'
  const relativePath = stripBase(route.path, siteBase)

  if (relativePath !== '/') {
    return
  }

  const targetLang = detectLanguage()
  const target = SUPPORTED_LANGS.find(lang => lang.code === targetLang) ?? SUPPORTED_LANGS[0]
  const targetPath = resolveLanguageTarget({
    routePath: route.path,
    siteBase,
    targetLangPath: target.path,
  })

  if (normalizePath(route.path) !== normalizePath(targetPath)) {
    router.go(targetPath)
  }
})
</script>

<template>
  <!-- Redirect logic only -->
</template>
