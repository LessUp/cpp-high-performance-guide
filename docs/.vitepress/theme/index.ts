import { h } from 'vue'
import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'
import LanguageRedirect from './LanguageRedirect.vue'
import LanguageSwitcher from './LanguageSwitcher.vue'
import MetricStrip from './MetricStrip.vue'
import SectionHero from './SectionHero.vue'
import SectionIndex from './SectionIndex.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('SectionHero', SectionHero)
    app.component('MetricStrip', MetricStrip)
    app.component('SectionIndex', SectionIndex)
  },
  Layout() {
    return h(DefaultTheme.Layout, null, {
      'layout-top': () => h(LanguageRedirect),
      'nav-bar-content-after': () => h(LanguageSwitcher),
    })
  },
} satisfies Theme
