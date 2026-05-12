<script setup lang="ts">
import { onMounted } from 'vue'
import { useRouter, useRoute, useData } from 'vitepress'

const STORAGE_KEY = 'cpp-hpc-guide-lang-preference'
const DEFAULT_LANG = 'en'

const SUPPORTED_LANGS = [
  { code: 'en', path: '/en/', match: ['en', 'en-US', 'en-GB', 'en-CA', 'en-AU'] },
  { code: 'zh', path: '/zh/', match: ['zh', 'zh-CN', 'zh-TW', 'zh-HK', 'zh-SG'] },
]

function getBrowserLanguage(): string {
  if (typeof navigator === 'undefined') return DEFAULT_LANG
  return navigator.language || (navigator as any).userLanguage || DEFAULT_LANG
}

function detectLanguage(): string {
  const stored = typeof localStorage !== 'undefined' ? localStorage.getItem(STORAGE_KEY) : null
  if (stored) return stored

  const browserLang = getBrowserLanguage()
  for (const lang of SUPPORTED_LANGS) {
    if (lang.match.some(m => browserLang.toLowerCase().startsWith(m.toLowerCase()))) {
      return lang.code
    }
  }
  return DEFAULT_LANG
}

function getLanguagePath(base: string, langCode: string): string {
  const lang = SUPPORTED_LANGS.find(l => l.code === langCode)
  const langPath = lang?.path || `/${langCode}/`
  // Combine base path with language path, avoiding double slashes
  if (base === '/') return langPath
  const cleanBase = base.endsWith('/') ? base.slice(0, -1) : base
  return `${cleanBase}${langPath}`
}

onMounted(() => {
  // Only run on client side
  if (typeof window === 'undefined') return

  const router = useRouter()
  const route = useRoute()
  const { site } = useData()

  const base = site.value.base || '/'
  const currentPath = route.path

  // Only redirect from root path (not already on a language-specific path)
  // Handle both `/` and `/cpp-high-performance-guide/` cases
  const isRoot = currentPath === '/' || currentPath === base || currentPath === base.replace(/\/$/, '')
  if (!isRoot) {
    return
  }

  const targetLang = detectLanguage()
  const targetPath = getLanguagePath(base, targetLang)

  // Avoid redirect loop - only redirect if we're not already at the target
  if (!currentPath.startsWith(targetPath.replace(/\/$/, ''))) {
    // Use replace to avoid adding a history entry
    router.go(targetPath)
  }
})
</script>

<template>
  <!-- This component has no visual representation -->
</template>
