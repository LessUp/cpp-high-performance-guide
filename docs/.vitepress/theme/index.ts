import { h } from 'vue'
import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'
import LanguageRedirect from './LanguageRedirect.vue'
import LanguageSwitcher from './LanguageSwitcher.vue'

export default {
  extends: DefaultTheme,
  Layout() {
    return h(DefaultTheme.Layout, null, {
      'layout-top': () => h(LanguageRedirect),
      'nav-bar-content-after': () => h(LanguageSwitcher),
    })
  },
} satisfies Theme
