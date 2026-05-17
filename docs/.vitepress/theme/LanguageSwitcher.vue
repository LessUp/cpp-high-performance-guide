<script setup lang="ts">
import { computed, onBeforeUnmount, onMounted, ref } from 'vue'
import { useData, useRoute } from 'vitepress'
import { createLanguageSwitcherLinks } from './language-routing.js'

const STORAGE_KEY = 'cpp-hpc-guide-lang-preference'

const { site, theme } = useData()
const route = useRoute()
const isOpen = ref(false)
const rootRef = ref<HTMLDetailsElement | null>(null)

const languageLinks = computed(() => createLanguageSwitcherLinks({
  routePath: route.path,
  siteBase: site.value.base || '/',
}))

const currentLang = computed(() => languageLinks.value.find(lang => lang.isCurrent) ?? languageLinks.value[0])

function closeMenu() {
  rootRef.value?.removeAttribute('open')
  isOpen.value = false
}

function rememberLangPreference(langCode: string) {
  if (typeof localStorage !== 'undefined') {
    localStorage.setItem(STORAGE_KEY, langCode)
  }
}

function syncOpenState() {
  isOpen.value = rootRef.value?.open ?? false
}

function onDocumentClick(event: MouseEvent) {
  if (!rootRef.value?.contains(event.target as Node)) {
    closeMenu()
  }
}

function onDocumentKeydown(event: KeyboardEvent) {
  if (event.key === 'Escape') {
    closeMenu()
  }
}

onMounted(() => {
  document.addEventListener('click', onDocumentClick)
  document.addEventListener('keydown', onDocumentKeydown)
})

onBeforeUnmount(() => {
  document.removeEventListener('click', onDocumentClick)
  document.removeEventListener('keydown', onDocumentKeydown)
})
</script>

<template>
  <details ref="rootRef" class="language-switcher" @toggle="syncOpenState">
    <summary
      class="language-button"
      :title="theme.langMenuLabel || 'Switch Language'"
    >
      <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75">
        <circle cx="12" cy="12" r="10" />
        <path d="M2 12h20" />
        <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z" />
      </svg>
      <span class="language-label">{{ currentLang.label }}</span>
      <svg class="chevron" :class="{ open: isOpen }" width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75">
        <polyline points="6 9 12 15 18 9" />
      </svg>
    </summary>

    <ul class="language-dropdown language-list">
      <li
          v-for="lang in languageLinks"
          :key="lang.code"
      >
        <a
          class="language-option"
          :class="{ active: lang.isCurrent }"
          :href="lang.targetPath"
          @click="rememberLangPreference(lang.code)"
        >
          <span class="option-label">{{ lang.label }}</span>
          <svg v-if="lang.isCurrent" class="check-icon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75">
            <polyline points="20 6 9 17 4 12" />
          </svg>
        </a>
      </li>
    </ul>
  </details>
</template>

<style scoped>
.language-switcher {
  position: relative;
  display: inline-flex;
  align-items: center;
}

.language-button {
  list-style: none;
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  min-height: 2.5rem;
  padding: 0.45rem 0.8rem;
  border: 1px solid var(--wp-pill-border);
  border-radius: 999px;
  background: var(--wp-pill-bg);
  color: var(--wp-pill-text);
  font-size: 0.86rem;
  font-weight: 600;
  cursor: pointer;
  transition: border-color var(--wp-motion-1), color var(--wp-motion-1), transform var(--wp-motion-1);
  white-space: nowrap;
}

.language-button::-webkit-details-marker {
  display: none;
}

.language-button:hover {
  border-color: color-mix(in oklch, var(--wp-accent-1) 48%, var(--wp-line-2));
  color: var(--wp-accent-1);
  transform: translateY(-1px);
}

.language-button svg,
.language-option svg {
  color: var(--wp-icon-muted);
}

.language-button:hover svg,
.language-option.active svg {
  color: currentColor;
}

.language-label {
  display: none;
}

.chevron {
  transition: transform 0.16s ease;
}

.chevron.open {
  transform: rotate(180deg);
}

.language-switcher:not([open]) .language-dropdown {
  display: none;
}

.language-dropdown {
  position: absolute;
  top: calc(100% + 0.4rem);
  right: 0;
  min-width: 8.5rem;
  padding: 0.35rem;
  border: 1px solid var(--wp-line-1);
  border-radius: 1rem;
  background: var(--wp-surface-1);
  box-shadow: var(--wp-shadow-1);
  z-index: 100;
}

.language-list {
  margin: 0;
  padding: 0;
  list-style: none;
}

.language-option {
  display: flex;
  align-items: center;
  justify-content: space-between;
  width: 100%;
  padding: 0.65rem 0.8rem;
  border-radius: 0.75rem;
  background: transparent;
  color: var(--wp-ink-2);
  font-size: 0.92rem;
  font-weight: 500;
  text-decoration: none;
  cursor: pointer;
  transition: background var(--wp-motion-1), color var(--wp-motion-1);
}

.language-option:hover,
.language-option:focus-visible,
.language-option.active {
  background: var(--wp-meta-bg);
  color: var(--wp-accent-1);
}

@media (min-width: 768px) {
  .language-label {
    display: inline;
  }
}
</style>
