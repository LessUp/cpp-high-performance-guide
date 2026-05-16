<script setup lang="ts">
import { ref, computed } from 'vue'
import { useRouter, useRoute, useData } from 'vitepress'
import { SUPPORTED_LANGS, resolveLanguageTarget, stripBase } from './language-routing.js'

const STORAGE_KEY = 'cpp-hpc-guide-lang-preference'

const { site, theme } = useData()
const router = useRouter()
const route = useRoute()
const isOpen = ref(false)

const currentLang = computed(() => {
  const path = stripBase(route.path, site.value.base || '/')
  const lang = SUPPORTED_LANGS.find(l => path.startsWith(l.path))
  return lang || SUPPORTED_LANGS[0]
})

function switchLang(lang: typeof SUPPORTED_LANGS[0]) {
  if (lang.code === currentLang.value.code) {
    isOpen.value = false
    return
  }

  // Save preference
  if (typeof localStorage !== 'undefined') {
    localStorage.setItem(STORAGE_KEY, lang.code)
  }

  // Calculate target path
  const targetPath = resolveLanguageTarget({
    routePath: route.path,
    siteBase: site.value.base || '/',
    targetLangPath: lang.path,
  })

  isOpen.value = false
  router.go(targetPath)
}

function toggle() {
  isOpen.value = !isOpen.value
}

function handleClickOutside(event: MouseEvent) {
  const target = event.target as HTMLElement
  if (!target.closest('.language-switcher')) {
    isOpen.value = false
  }
}

// Close on outside click
if (typeof document !== 'undefined') {
  document.addEventListener('click', handleClickOutside)
}
</script>

<template>
  <div class="language-switcher">
    <button class="language-button" @click="toggle" :title="theme.langMenuLabel || 'Switch Language'">
      <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <path d="M2 12h20"/>
        <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/>
      </svg>
      <span class="language-label">{{ currentLang.label }}</span>
      <svg class="chevron" :class="{ open: isOpen }" width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <polyline points="6 9 12 15 18 9"/>
      </svg>
    </button>

    <Transition name="dropdown">
      <div v-if="isOpen" class="language-dropdown">
        <button
          v-for="lang in SUPPORTED_LANGS"
          :key="lang.code"
          class="language-option"
          :class="{ active: lang.code === currentLang.code }"
          @click="switchLang(lang)"
        >
          <span class="option-label">{{ lang.label }}</span>
          <svg v-if="lang.code === currentLang.code" class="check-icon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <polyline points="20 6 9 17 4 12"/>
          </svg>
        </button>
      </div>
    </Transition>
  </div>
</template>

<style scoped>
.language-switcher {
  position: relative;
  display: inline-flex;
  align-items: center;
}

.language-button {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  padding: 6px 10px;
  border: 1px solid var(--vp-c-border);
  border-radius: 6px;
  background: transparent;
  color: var(--vp-c-text-2);
  font-size: 13px;
  cursor: pointer;
  transition: all 0.15s ease;
  white-space: nowrap;
}

.language-button:hover {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
}

.language-label {
  display: none;
}

@media (min-width: 768px) {
  .language-label {
    display: inline;
  }
}

.chevron {
  transition: transform 0.2s ease;
}

.chevron.open {
  transform: rotate(180deg);
}

.language-dropdown {
  position: absolute;
  top: 100%;
  right: 0;
  margin-top: 4px;
  min-width: 120px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-border);
  border-radius: 8px;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
  overflow: hidden;
  z-index: 100;
}

.language-option {
  display: flex;
  align-items: center;
  justify-content: space-between;
  width: 100%;
  padding: 10px 14px;
  border: none;
  background: transparent;
  color: var(--vp-c-text-1);
  font-size: 14px;
  cursor: pointer;
  transition: background 0.15s ease;
}

.language-option:hover {
  background: var(--vp-c-bg-soft);
}

.language-option.active {
  color: var(--vp-c-brand-1);
  background: var(--vp-c-brand-soft);
}

.check-icon {
  color: var(--vp-c-brand-1);
}

/* Dropdown transition */
.dropdown-enter-active,
.dropdown-leave-active {
  transition: opacity 0.15s ease, transform 0.15s ease;
}

.dropdown-enter-from,
.dropdown-leave-to {
  opacity: 0;
  transform: translateY(-4px);
}
</style>
